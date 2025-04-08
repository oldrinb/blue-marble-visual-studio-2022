/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 30, 2025
 **/

#include "BaseLight.h"



const float BaseLight::kLengthEpsilon_ = 0.01f;



BaseLight::~BaseLight() {
	//cout << "[Base light " << kId_ << "] deleted." << endl;
}



void BaseLight::setCameraViewMatrix(const mat4& kMatrix) {
	cameraViewMatrix_ = kMatrix;
	updateLightInEyeSpace_();
}



void BaseLight::setAmbientColor(const vec3& kColor) {
	if (std::abs(kColor.r - 0.5f) <= 0.5f && std::abs(kColor.g - 0.5f) <= 0.5f && std::abs(kColor.b - 0.5f) <= 0.5f)
		ambientColor_ = kColor;
	else throw runtime_error("BaseLight.setAmbientColor|Invalid ambient color value.");
}



void BaseLight::setDiffuseColor(const vec3& kColor) {
	if (std::abs(kColor.r - 0.5f) <= 0.5f && std::abs(kColor.g - 0.5f) <= 0.5f && std::abs(kColor.b - 0.5f) <= 0.5f)
		diffuseColor_ = kColor;
	else throw runtime_error("BaseLight.setDiffuseColor|Invalid diffuse color value.");
}



void BaseLight::setSpecularColor(const vec3& kColor) {
	if (std::abs(kColor.r - 0.5f) <= 0.5f && std::abs(kColor.g - 0.5f) <= 0.5f && std::abs(kColor.b - 0.5f) <= 0.5f)
		specularColor_ = kColor;
	else throw runtime_error("BaseLight.setSpecularColor|Invalid specular color value.");
}



void BaseLight::setOn() {
	on_ = true;
}



void BaseLight::setOff() {
	on_ = false;
}



const vec3* BaseLight::getAmbientColor() const {
	return &ambientColor_;
}



const vec3* BaseLight::getDiffuseColor() const {
	return &diffuseColor_;
}



const vec3* BaseLight::getSpecularColor() const {
	return &specularColor_;
}



bool BaseLight::isOn() const {
	return on_;
}



unsigned int BaseLight::getId() const {
	return kId_;
}



BaseLight::BaseLight(unsigned int id, const vec3& kAmbientColor, const vec3& kDiffuseColor, const vec3& kSpecularColor,
	                 bool on) : kId_(id),
	                 worldPosition_(vec3(0.0f, 0.0f, 1.0f)), worldLookAt_(vec3(0.0f, 0.0f, 0.0f)),
	                 worldDirection_(worldLookAt_ - worldPosition_),
	                 eyePosition_(vec3(worldPosition_)), eyeLookAt_(vec3(worldLookAt_)), eyeDirection_(worldDirection_),
	                 ambientColor_(vec3(0.0f)), diffuseColor_(vec3(0.0f)), specularColor_(vec3(0.0f)),
	                 cameraViewMatrix_(mat4(1.0f)),  on_(on) {

	try {
		setAmbientColor(kAmbientColor);
		setDiffuseColor(kDiffuseColor);
		setSpecularColor(kSpecularColor);
	}
	catch (const exception& kException) {
		throw runtime_error("BaseLight > " + string(kException.what()));
	}

	//cout << "[Base light " << kId_ << "] created." << endl;
}



void BaseLight::updateLightInEyeSpace_() {
	eyePosition_ = vec3(cameraViewMatrix_ * vec4(worldPosition_, 1.0f));
	eyeLookAt_ = vec3(cameraViewMatrix_ * vec4(worldLookAt_, 1.0f));
	eyeDirection_ = eyeLookAt_ - eyePosition_;
	if (glm::length(eyeDirection_) > BaseLight::kLengthEpsilon_)
		eyeDirection_ = glm::normalize(eyeDirection_);
}
