/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 30, 2025
 **/

#ifndef DIRECTIONAL_LIGHT_H
#define DIRECTIONAL_LIGHT_H

#include "BaseLight.h"

#include <glm/geometric.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

using glm::uvec2;
using glm::vec3;

using std::cout;
using std::endl;
using std::exception;
using std::runtime_error;
using std::string;



class DirectionalLight : public BaseLight {
public:
	//-> enum class BaseLight::LightType { DIRECTIONAL = 1u, POINT = 2u, SPOT = 3u };
	//-> enum class BaseLight::LightComponent { AMBIENT = 1u, DIFFUSE = 2u, SPECULAR = 3u, EMISSIVE = 4u };

	//-> static const unsigned int BaseLight::MAX_NUMBER_OF_LIGHTS = 5u;


	DirectionalLight(unsigned int id, const vec3& kWorldDirection, 
		             const vec3& kAmbientColor, const vec3& kDiffuseColor, const vec3& kSpecularColor,
		             bool on = true);
	virtual ~DirectionalLight();


	// set
	//############################################################################
	void setWorldDirection(const vec3& kDirection);

	//-> void setCameraViewMatrix(const mat4& kMatrix);

	//-> void setAmbientColor(const vec3& kColor);
	//-> void setDiffuseColor(const vec3& kColor);
	//-> void setSpecularColor(const vec3& kColor);

	//-> void setOn();
	//-> void setOff();


	// get
	//############################################################################
	const vec3* getWorldDirection() const;
	const vec3* getEyeDirection() const;

	//-> const vec3* getAmbientColor() const;
	//-> const vec3* getDiffuseColor() const;
	//-> const vec3* getSpecularColor() const;

	//-> bool isOn() const;

	virtual BaseLight::LightType getType() const;
	//-> unsigned int getId() const;	

private:
	DirectionalLight(const DirectionalLight&);
	const DirectionalLight& operator=(const DirectionalLight&) {}
};

#endif
