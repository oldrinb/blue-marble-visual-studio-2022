/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 26, 2021
 **/

#ifndef MATERIAL_H
#define MATERIAL_H

#include "texture/texture/ColorTexture.h"

#include <glm/vec3.hpp>

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

using glm::vec3;

using std::cout;
using std::endl;
using std::runtime_error;
using std::string;
using std::to_string;



class Material {
public:
	// defined also in 'shaders/main/shading.glsl'
	enum class ShadingModel { NO_SHADING = 1u, GOURAUD = 3u, PHONG = 4u };


	Material(unsigned int id, unsigned int modelId);
	~Material ();


	// set
	//############################################################################	
	void setShadingModel(Material::ShadingModel shadingModel);
	
	void setAmbientColor(const vec3& kColor);
	void setDiffuseColor(const vec3& kColor);
	void setSpecularColor(const vec3& kColor);
	void setEmissiveColor(const vec3& kColor);

	void setDiffuseTexture(const ColorTexture* pkTexture);
	void setSpecularTexture(const ColorTexture* pkTexture);
	void setEmissiveTexture(const ColorTexture* pkTexture);
	void setNormalMapTexture(const ColorTexture* pkTexture);
	
	void setShininess(float shininess);
	void setShininessStrength(float strength);

	void setOpacity(float opacity);

	void setTwoSided(bool twoSided);
	void setWireframe(bool wireframe);


	// get
	//############################################################################	
	Material::ShadingModel getShadingModel() const;

	const vec3* getAmbientColor() const;
	const vec3* getDiffuseColor() const;
	const vec3* getSpecularColor() const;
	const vec3* getEmissiveColor() const;

	const ColorTexture* getDiffuseTexture() const;
	const ColorTexture* getSpecularTexture() const;
	const ColorTexture* getEmissiveTexture() const;
	const ColorTexture* getNormalMapTexture() const;
	
	float getShininess() const;
	float getShininessStrength() const;

	float getOpacity() const;

	bool isTwoSided() const;
	bool isWireframe() const;
	bool isTransparent() const;
	
	unsigned int getId() const;
	unsigned int getModelId() const;	
	string toString() const;
	
private:
	static float getMaxShininess_();

	Material(const Material&);
	const Material& operator=(const Material&) {}

	const unsigned int kId_, kModelId_;
	const ColorTexture* pkDiffuseTexture_, * pkSpecularTexture_, * pkEmissiveTexture_, * pkNormalMapTexture_;
	const vec3 kBlack_;

	Material::ShadingModel shadingModel_;
	vec3 ambientColor_, diffuseColor_, specularColor_, emissiveColor_;	

	float shininess_, shininessStrength_, opacity_;
	bool twoSided_, wireframe_;
};

#endif
