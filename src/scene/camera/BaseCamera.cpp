/**
 * Author: Oldrin Barbulescu
 * Last modified: Feb 16, 2021
 **/

#include "BaseCamera.h"



const float BaseCamera::kLengthEpsilon_ = 0.01f;
const float BaseCamera::kDetEpsilon_ = 0.000001f;



float BaseCamera::getDefaultNearPlane_() {
	return 0.01f;
}



float BaseCamera::getDefaultFarPlane_() {
	return 100.0f;
}



double BaseCamera::getMaxVerticalAngle_() { // radians
	return 1.55334303427; // 89 degrees
}



BaseCamera::~BaseCamera() {
	//cout << "[Base camera " << kId_ << "] deleted." << endl;
}



void BaseCamera::setPosition(const vec3& kPosition) {
	if (glm::distance (kPosition, lookAt_) < BaseCamera::kLengthEpsilon_)
		throw runtime_error("BaseCamera.setPosition|Invalid camera position value.");

	position_ = kPosition;

	try {
		updateAngles_();
		updateDirections_(false);
		updateViewMatrix_();
	}
	catch (const exception& kException) {
		throw runtime_error("BaseCamera.setPosition > " + string(kException.what()));
	}
}



void BaseCamera::setLookAt(const vec3& kLookAt) {
	if (glm::distance(position_, kLookAt) < BaseCamera::kLengthEpsilon_)
		throw runtime_error("BaseCamera.setLookAt|Invalid camera 'look at' value.");
		
	lookAt_ = kLookAt;
	
	try {
		updateAngles_();
		updateDirections_(false);
		updateViewMatrix_();
	}
	catch (const exception& kException) {
		throw runtime_error("BaseCamera.setLookAt > " + string(kException.what()));
	}
}



void BaseCamera::setUp(const vec3& kUp) {
	if (glm::abs(kUp) != vec3(1.0f, 0.0f, 0.0f) &&
		glm::abs(kUp) != vec3(0.0f, 1.0f, 0.0f) &&
		glm::abs(kUp) != vec3(0.0f, 0.0f, 1.0f))
	throw runtime_error("BaseCamera.setUp|Invalid camera 'up' vector value.");	
	up_ = kUp;

	try {
		updateAngles_();
		updateDirections_(false);
		updateViewMatrix_();
	}
	catch (const exception& kException) {
		throw runtime_error("BaseCamera.setUp > " + string(kException.what()));
	}
}



void BaseCamera::setNearPlane(float nearPlane) {
	if (nearPlane < 0.0f || nearPlane >= farPlane_)
		throw runtime_error("BaseCamera.setNearPlane|Invalid near plane value.");
	nearPlane_ = nearPlane;

	try {
		updateProjectionMatrix_();
	}
	catch (const exception& kException) {
		throw runtime_error("BaseCamera.setNearPlane > " + string(kException.what()));
	}
}



void BaseCamera::setFarPlane(float farPlane) {
	if (farPlane <= nearPlane_)
		throw runtime_error("BaseCamera.setFarPlane|Invalid far plane value.");
	farPlane_ = farPlane;

	try {
		updateProjectionMatrix_();
	}
	catch (const exception& kException) {
		throw runtime_error("BaseCamera.setFarPlane > " + string(kException.what()));
	}
}



void BaseCamera::setNearAndFarPlanes(float nearPlane, float farPlane) {
	if (nearPlane < 0.0f || nearPlane >= farPlane)
		throw runtime_error("BaseCamera.setNearAndFarPlanes|Invalid near plane and far plane values.");
	nearPlane_ = nearPlane;
	farPlane_ = farPlane;

	try {
		updateProjectionMatrix_();
	}
	catch (const exception& kException) {
		throw runtime_error("BaseCamera.setNearAndFarPlanes > " + string(kException.what()));
	}
}



void BaseCamera::translateRight(float distance) {
	position_ += right_ * distance;
	lookAt_ += right_ * distance;

	try {
		updateViewMatrix_();
	}
	catch (const exception& kException) {
		throw runtime_error("BaseCamera.translateRight > " + string(kException.what()));
	}
}



void BaseCamera::translateUp(float distance) {
	position_ += top_ * distance;
	lookAt_ += top_ * distance;

	try {
		updateViewMatrix_();
	}
	catch (const exception& kException) {
		throw runtime_error("BaseCamera.translateUp > " + string(kException.what()));
	}
}



void BaseCamera::translateForward(float distance) {
	position_ += forward_ * distance;
	lookAt_ += forward_ * distance;

	try {
		updateViewMatrix_();
	}
	catch (const exception& kException) {
		throw runtime_error("BaseCamera.translateForward > " + string(kException.what()));
	}
}



void BaseCamera::translateToCenter(float distance) {
	vec3 newPosition = position_ + forward_ * distance;
	if (glm::dot(forward_, lookAt_ - newPosition) > 0.0f) {
		position_ = newPosition;
		ro_ = glm::distance(position_, lookAt_);

		try {
			updateViewMatrix_();
		}
		catch (const exception& kException) {
			throw runtime_error("BaseCamera.translateToCenter > " + string(kException.what()));
		}
	}
}



void BaseCamera::rotateRight(float angle) { // degrees
	if (up_.x + up_.y + up_.z == 1.0f)
		horizontalAngle_ += glm::radians(angle);
	else horizontalAngle_ -= glm::radians(angle);
	
	horizontalAngle_ = glm::pi<double>() + std::remainder(horizontalAngle_ - glm::pi<double>(), 2.0 * glm::pi<double>());

	try {
		updateDirections_(true);
		updateViewMatrix_();
	}
	catch (const exception& kException) {
		throw runtime_error("BaseCamera.rotateRight > " + string(kException.what()));
	}
}



void BaseCamera::rotateUp(float angle) { // degrees
	double newAngle = verticalAngle_;

	if (up_.x + up_.y + up_.z == 1.0f)
		newAngle += glm::radians(angle);
	else newAngle -= glm::radians(angle);

	if (glm::abs(newAngle) <= BaseCamera::getMaxVerticalAngle_())
		verticalAngle_ = newAngle;

	try {
		updateDirections_(true);
		updateViewMatrix_();
	}
	catch (const exception& kException) {
		throw runtime_error("BaseCamera.rotateUp > " + string(kException.what()));
	}
}



void BaseCamera::reset() {
	position_ = origPosition_;
	lookAt_ = origLookAt_;

	try {
		updateAngles_();
		updateDirections_(false);
		updateViewMatrix_();
	}
	catch (const exception& kException) {
		throw runtime_error("BaseCamera.reset > " + string(kException.what()));
	}
}



const vec3* BaseCamera::getPosition() const {
	return &position_;
}



const vec3* BaseCamera::getLookAt() const {
	return &lookAt_;
}



const vec3* BaseCamera::getUp() const {
	return &up_;
}



float BaseCamera::getNearPlane() const {
	return nearPlane_;
}



float BaseCamera::getFarPlane() const {
	return farPlane_;
}



const mat4* BaseCamera::getViewMatrix() const {
	return &viewMatrix_;
}



const mat4*  BaseCamera::getProjectionMatrix() const {
	return &projectionMatrix_;
}



const mat4* BaseCamera::getViewProjectionMatrix() const {
	return &viewProjectionMatrix_;
}



unsigned int BaseCamera::getId() const {
	return kId_;
}



BaseCamera::BaseCamera(unsigned int id, const vec3& kPosition, const vec3& kLookAt) :
	                   BaseCamera(id, kPosition, kLookAt, vec3(0.0f, 1.0f, 0.0f),
					   BaseCamera::getDefaultNearPlane_(), BaseCamera::getDefaultFarPlane_()) {}



BaseCamera::BaseCamera(unsigned int id, const vec3& kPosition, const vec3& kLookAt, const vec3& kUp):
	                   BaseCamera(id, kPosition, kLookAt, kUp, 
					   BaseCamera::getDefaultNearPlane_(), BaseCamera::getDefaultFarPlane_()) {}



BaseCamera::BaseCamera(unsigned int id, const vec3& kPosition, const vec3& kLookAt, const vec3& kUp,
	                   float nearPlane, float farPlane): kId_(id), 
	                   viewMatrix_(mat4(1.0f)), projectionMatrix_(mat4(1.0f)), viewProjectionMatrix_(mat4(1.0f)),
	                   nearPlane_(nearPlane), farPlane_(farPlane),
	                   origPosition_(kPosition), position_(kPosition), origLookAt_(kLookAt), lookAt_(kLookAt), up_(kUp),
	                   forward_(vec3(0.0f, 0.0f, -1.0f)), right_(vec3(1.0f, 0.0f, 0.0f)), top_(vec3(0.0f, 1.0f, 0.0f)),	                   
	                   ro_(1.0f), horizontalAngle_(0.0), verticalAngle_(0.0) {
	if (glm::distance(kPosition, kLookAt) < BaseCamera::kLengthEpsilon_)
		throw runtime_error("BaseCamera|Camera position and 'look at' are identical.");

	if (glm::abs(kUp) != vec3(1.0f, 0.0f, 0.0f) &&
		glm::abs(kUp) != vec3(0.0f, 1.0f, 0.0f) &&
		glm::abs(kUp) != vec3(0.0f, 0.0f, 1.0f))
		throw runtime_error("BaseCamera|Invalid camera 'up' vector value.");

	if (nearPlane < 0.0f || nearPlane >= farPlane)
		throw runtime_error("BaseCamera|Invalid near plane and far plane values.");

	try {
		updateAngles_();
		updateDirections_(false);
		updateViewMatrix_();
	}
	catch (const exception& kException) {
		throw runtime_error("BaseCamera > " + string(kException.what()));
	}

	//cout << "[Base camera " << id << "] created." << endl;
}



void BaseCamera::updateProjectionMatrix_() {
	viewProjectionMatrix_ = projectionMatrix_ * viewMatrix_;
}



void BaseCamera::updateViewMatrix_() {
	viewMatrix_ = glm::lookAt(position_, lookAt_, up_);

	float det = glm::determinant(viewMatrix_);
	if (glm::isnan(det) || glm::abs(det) <= BaseCamera::kDetEpsilon_)
		throw runtime_error("BaseCamera.updateViewMatrix_|Invalid view matrix.");

	viewProjectionMatrix_ = projectionMatrix_ * viewMatrix_;
}



void BaseCamera::updateAngles_() {
	forward_ = lookAt_ - position_;
	ro_ = glm::length(forward_);
	forward_ = glm::normalize(forward_);

	vec3 fv = vec3(0.0f);
	if (glm::abs(up_.y) == 1.0f) fv = forward_;
	else if (glm::abs(up_.x) == 1.0f)
		fv = vec3(forward_.z, forward_.x, forward_.y);
	else if (glm::abs(up_.z) == 1.0f)
		fv = vec3(forward_.y, forward_.z, forward_.x);

	verticalAngle_ = glm::asin(-fv.y);
	if (glm::abs(verticalAngle_) > BaseCamera::getMaxVerticalAngle_())
		throw runtime_error("BaseCamera.updateAngles_|Invalid vertical angle value.");

	horizontalAngle_ = glm::acos(glm::clamp(-fv.z / glm::cos(verticalAngle_), -1.0, 1.0));
	if (fv.x > 0.0f) horizontalAngle_ = 2.0 * glm::pi<double>() - horizontalAngle_;
}



void BaseCamera::updateDirections_(bool updateForward) {
	if (updateForward) {
		forward_ = vec3(-glm::cos(verticalAngle_) * glm::sin(horizontalAngle_),
			            -glm::sin(verticalAngle_),
			            -glm::cos(verticalAngle_) * glm::cos(horizontalAngle_));

		if (glm::abs(up_.x) == 1.0f)
			forward_ = vec3(forward_.y, forward_.z, forward_.x);
		else if (glm::abs(up_.z) == 1.0f)
			forward_ = vec3(forward_.z, forward_.x, forward_.y);

		position_ = lookAt_ - ro_ * forward_;
	}

	if (glm::abs(up_.x) == 1.0f)
		right_ = glm::normalize(vec3(0.0f, forward_.z, -forward_.y));
	else if (glm::abs(up_.y) == 1.0f)
		right_ = glm::normalize(vec3(-forward_.z, 0.0f, forward_.x));
	else if (glm::abs(up_.z) == 1.0f)
		right_ = glm::normalize(vec3(forward_.y, -forward_.x, 0.0f));

	if (up_.x + up_.y + up_.z == -1.0f)		
		right_ = -right_;

	top_ = glm::cross(right_, forward_);
}
