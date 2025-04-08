/**
 * Author: Oldrin Barbulescu
 * Last modified: Feb 17, 2021
 **/
 
#ifndef BASE_CAMERA_H
#define BASE_CAMERA_H

#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/mat4x4.hpp>
#include <glm/matrix.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>

#include <cmath>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

using glm::mat4;
using glm::vec3;

using std::cout;
using std::endl;
using std::exception;
using std::runtime_error;
using std::string;



class BaseCamera {
public:
	enum class CameraType { ORTHOGRAPHIC = 1u, PERSPECTIVE = 2u };
	static const unsigned int MAX_NUMBER_OF_CAMERAS = 5u;

	virtual ~BaseCamera();

	void setPosition(const vec3& kPosition);
	void setLookAt(const vec3& kLookAt);
	void setUp(const vec3& kUp);

	void setNearPlane(float nearPlane);
	void setFarPlane(float farPlane);
	void setNearAndFarPlanes(float nearPlane, float farPlane);

	void translateRight(float distance);
	void translateUp(float distance);
	void translateForward(float distance);
	void translateToCenter(float distance);

	void rotateRight(float angle); // degrees
	void rotateUp(float angle); // degrees

	void reset();

	const vec3* getPosition() const;
	const vec3* getLookAt() const;
	const vec3* getUp() const;

	float getNearPlane() const;
	float getFarPlane() const;

	const mat4* getViewMatrix() const;
	const mat4* getProjectionMatrix() const;
	const mat4* getViewProjectionMatrix() const;

	virtual BaseCamera::CameraType getType() const = 0;
	unsigned int getId() const;

protected:
	static const float kLengthEpsilon_;
	static const float kDetEpsilon_;

	static float getDefaultNearPlane_();
	static float getDefaultFarPlane_();

	BaseCamera(unsigned int id, const vec3& kPosition, const vec3& kLookAt);
	BaseCamera(unsigned int id, const vec3& kPosition, const vec3& kLookAt, const vec3& kUp);
	BaseCamera(unsigned int id, const vec3& kPosition, const vec3& kLookAt, const vec3& kUp, float nearPlane, float farPlane);	
	
	virtual void updateProjectionMatrix_();

	const unsigned int kId_;

	mat4 viewMatrix_, projectionMatrix_, viewProjectionMatrix_;
	float nearPlane_, farPlane_;	

private:
	static double getMaxVerticalAngle_(); // degrees

	BaseCamera(const BaseCamera&);
	const BaseCamera& operator=(const BaseCamera&) {}

	void updateViewMatrix_();
	void updateAngles_();
	void updateDirections_(bool updateForward);

	vec3 origPosition_, position_, origLookAt_, lookAt_, up_;
	vec3 forward_, right_, top_;
	float ro_; 
	double horizontalAngle_, verticalAngle_;
};

#endif
