/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 30, 2025
 **/

#ifndef BASE_LIGHT_H
#define BASE_LIGHT_H

#include <glm/geometric.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cmath>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

using glm::mat4;
using glm::uvec2;
using glm::vec3;
using glm::vec4;

using std::cout;
using std::endl;
using std::exception;
using std::runtime_error;
using std::string;



class BaseLight {
public:
	enum class LightType { DIRECTIONAL = 1u, POINT = 2u, SPOT = 3u };
	enum class LightComponent { AMBIENT = 1u, DIFFUSE = 2u, SPECULAR = 3u, EMISSIVE = 4u };
	
	static const unsigned int MAX_NUMBER_OF_LIGHTS = 5u;

	virtual ~BaseLight();

	void setCameraViewMatrix(const mat4& kMatrix);

	void setAmbientColor(const vec3& kColor);
	void setDiffuseColor(const vec3& kColor);
	void setSpecularColor(const vec3& kColor);

	void setOn();
	void setOff();

	const vec3* getAmbientColor() const;
	const vec3* getDiffuseColor() const;
	const vec3* getSpecularColor() const;

	bool isOn() const;

	virtual BaseLight::LightType getType() const = 0;
	unsigned int getId() const;

protected:
	static const float kLengthEpsilon_;

	BaseLight(unsigned int id, const vec3& kAmbientColor, const vec3& kDiffuseColor, const vec3& kSpecularColor,
		      bool on = true);

	void updateLightInEyeSpace_();

	const unsigned int kId_;

	vec3 worldPosition_, worldLookAt_, worldDirection_;
	vec3 eyePosition_, eyeLookAt_, eyeDirection_;

private:
	BaseLight(const BaseLight&);
	const BaseLight& operator=(const BaseLight&) {}

	vec3 ambientColor_, diffuseColor_, specularColor_;
	mat4 cameraViewMatrix_;
	bool on_;
};

#endif
