/**
 * Author: Oldrin Barbulescu
 * Last modified: Feb 17, 2021
 **/
 
#include "PerspectiveCamera.h"



float PerspectiveCamera::getMinFieldOfView_() { // degrees
	return 0.0f;
}



float PerspectiveCamera::getMaxFieldOfView_() { // degrees
	return 180.0f;
}



PerspectiveCamera::PerspectiveCamera(unsigned int id, const vec3& kPosition, const vec3& kLookAt,
	                                 float fieldOfView, float aspectRatio) : BaseCamera(id, kPosition, kLookAt),
	                                 origFieldOfView_(glm::radians(fieldOfView)), fieldOfView_(origFieldOfView_),
	                                 aspectRatio_(aspectRatio) {

	if (fieldOfView <= PerspectiveCamera::getMinFieldOfView_() || fieldOfView >= PerspectiveCamera::getMaxFieldOfView_())
		throw runtime_error("PerspectiveCamera|Invalid field of view value.");
	if (aspectRatio <= 0.0f)
		throw runtime_error("PerspectiveCamera|Invalid aspect ratio value.");

	try {
		updateProjectionMatrix_();
	}
	catch (const exception& kException) {
		throw runtime_error("PerspectiveCamera > " + string(kException.what()));
	}

	//cout << "Perspective camera " << id << " created." << endl;
}



PerspectiveCamera::PerspectiveCamera(unsigned int id, const vec3& kPosition, const vec3& kLookAt, const vec3& kUp,
	                                 float fieldOfView, float aspectRatio) : BaseCamera(id, kPosition, kLookAt, kUp),
	                                 origFieldOfView_(glm::radians(fieldOfView)), fieldOfView_(origFieldOfView_),
	                                 aspectRatio_(aspectRatio) {

	if (fieldOfView <= PerspectiveCamera::getMinFieldOfView_() || fieldOfView >= PerspectiveCamera::getMaxFieldOfView_())
		throw runtime_error("PerspectiveCamera|Invalid field of view value.");
	if (aspectRatio <= 0.0f)
		throw runtime_error("PerspectiveCamera|Invalid aspect ratio value.");

	try {
		updateProjectionMatrix_();
	}
	catch (const exception& kException) {
		throw runtime_error("PerspectiveCamera > " + string(kException.what()));
	}

	//cout << "Perspective camera " << id << " created." << endl;
}



PerspectiveCamera::PerspectiveCamera(unsigned int id, const vec3& kPosition, const vec3& kLookAt, const vec3& kUp,
	                                 float fieldOfView, float aspectRatio, float nearPlane, float farPlane) :
	                                 BaseCamera(id, kPosition, kLookAt, kUp, nearPlane, farPlane),
	                                 origFieldOfView_(glm::radians(fieldOfView)), fieldOfView_(origFieldOfView_),
	                                 aspectRatio_(aspectRatio) {

	if (fieldOfView <= PerspectiveCamera::getMinFieldOfView_() || fieldOfView >= PerspectiveCamera::getMaxFieldOfView_())
		throw runtime_error("PerspectiveCamera|Invalid field of view value.");
	if (aspectRatio <= 0.0f)
		throw runtime_error("PerspectiveCamera|Invalid aspect ratio value.");

	try {
		updateProjectionMatrix_();
	}
	catch (const exception& kException) {
		throw runtime_error("PerspectiveCamera > " + string(kException.what()));
	}

	//cout << "Perspective camera " << id << " created." << endl;
}



PerspectiveCamera::~PerspectiveCamera() {
	//cout << "Perspective camera " << kId_ << " deleted." << endl;
}



void PerspectiveCamera::setFieldOfView(float fieldOfView) { // degrees
	if (fieldOfView <= PerspectiveCamera::getMinFieldOfView_() || fieldOfView >= PerspectiveCamera::getMaxFieldOfView_())
		throw runtime_error("PerspectiveCamera.setFieldOfView|Invalid field of view value.");
	fieldOfView_ = glm::radians(fieldOfView);

	try {
		updateProjectionMatrix_();
	}
	catch (const exception& kException) {
		throw runtime_error("PerspectiveCamera.setFieldOfView > " + string(kException.what()));
	}
}



void PerspectiveCamera::resetFieldOfView() {
	fieldOfView_ = origFieldOfView_;

	try {
		updateProjectionMatrix_();
	}
	catch (const exception& kException) {
		throw runtime_error("PerspectiveCamera.resetFieldOfView > " + string(kException.what()));
	}
}



void PerspectiveCamera::setAspectRatio(float aspectRatio) {
	if (aspectRatio <= 0.0f)
		throw runtime_error("PerspectiveCamera.setAspectRatio|Invalid aspect ratio value.");
	aspectRatio_ = aspectRatio;

	try {
		updateProjectionMatrix_();
	}
	catch (const exception& kException) {
		throw runtime_error("PerspectiveCamera.setAspectRatio > " + string(kException.what()));
	}
}



float PerspectiveCamera::getFieldOfView() const { // degrees
	return glm::degrees(fieldOfView_);
}



float PerspectiveCamera::getAspectRatio() const {
	return aspectRatio_;
}



BaseCamera::CameraType PerspectiveCamera::getType() const {
	return BaseCamera::CameraType::PERSPECTIVE;
}



void PerspectiveCamera::updateProjectionMatrix_() {
	float nearPlane = std::max(nearPlane_, BaseCamera::getDefaultNearPlane_());
	float farPlane = std::max(farPlane_, nearPlane_ + BaseCamera::getDefaultNearPlane_());

	projectionMatrix_ = glm::perspective(fieldOfView_, aspectRatio_, nearPlane, farPlane);

	float det = glm::determinant(projectionMatrix_);
	if (glm::isnan(det) || glm::abs(det) <= BaseCamera::kDetEpsilon_)
		throw runtime_error("PerspectiveCamera.updateProjectionMatrix_|Invalid projection matrix.");

	BaseCamera::updateProjectionMatrix_();
}
