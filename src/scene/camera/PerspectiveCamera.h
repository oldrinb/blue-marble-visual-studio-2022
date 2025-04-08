/**
 * Author: Oldrin Barbulescu
 * Last modified: Feb 17, 2021
 **/
 
#ifndef PERSPECTIVE_CAMERA_H
#define PERSPECTIVE_CAMERA_H

#include "BaseCamera.h"

#include <glm/common.hpp>
#include <glm/matrix.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>

#include <glm/ext/matrix_clip_space.hpp>

#include <algorithm>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

using glm::vec3;

using std::cout;
using std::endl;
using std::exception;
using std::runtime_error;
using std::string;



class PerspectiveCamera : public BaseCamera {
public:
	//-> enum class BaseCamera::CameraType { ORTHOGRAPHIC = 1u, PERSPECTIVE = 2u };
	//-> static const unsigned int BaseCamera::MAX_NUMBER_OF_CAMERAS = 5u;


	PerspectiveCamera(unsigned int id, const vec3& kPosition, const vec3& kLookAt, float fieldOfView, float aspectRatio);
	PerspectiveCamera(unsigned int id, const vec3& kPosition, const vec3& kLookAt, const vec3& kUp,
		              float fieldOfView, float aspectRatio);
	PerspectiveCamera(unsigned int id, const vec3& kPosition, const vec3& kLookAt, const vec3& kUp,
		              float fieldOfView, float aspectRatio, float nearPlane, float farPlane);
	virtual ~PerspectiveCamera();	


	// set
	//############################################################################
	//-> void setPosition(const vec3& kPosition);
	//-> void setLookAt(const vec3& kLookAt);
	//-> void setUp(const vec3& kUp);

	void setFieldOfView(float fieldOfView); // degrees
	void resetFieldOfView();

	void setAspectRatio(float aspectRatio);	

	//-> void setNearPlane(float nearPlane);
	//-> void setFarPlane(float farPlane);
	//-> void setNearAndFarPlanes(float nearPlane, float farPlane);

	//-> void translateRight(float distance);
	//-> void translateUp(float distance);
	//-> void translateForward(float distance);
	//-> void translateToCenter(float distance);

	//-> void rotateRight(float angle); // degrees
	//-> void rotateUp(float angle); // degrees

	//-> void reset();


	// get
	//############################################################################
	//-> const vec3* getPosition() const;
	//-> const vec3* getLookAt() const;
	//-> const vec3* getUp() const;

	float getFieldOfView() const; // degrees
	float getAspectRatio() const;

	//-> float getNearPlane() const;
	//-> float getFarPlane() const;

	//-> const mat4* getViewMatrix() const;
	//-> const mat4* getProjectionMatrix() const;
	//-> const mat4* getViewProjectionMatrix() const;

	virtual BaseCamera::CameraType getType() const;
	//-> unsigned int getId() const;

private:
	static float getMinFieldOfView_(); // degrees
	static float getMaxFieldOfView_(); // degrees

	PerspectiveCamera(const PerspectiveCamera&);
	const PerspectiveCamera& operator=(const PerspectiveCamera&) {}

	virtual void updateProjectionMatrix_();

	float origFieldOfView_, fieldOfView_, aspectRatio_;
};

#endif
