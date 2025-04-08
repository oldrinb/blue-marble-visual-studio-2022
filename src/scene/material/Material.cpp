/**
 * Author: Oldrin Barbulescu
 * Last modified: Feb 2, 2021
 **/

#include "Material.h"



float Material::getMaxShininess_() {
	return 128.0f;
}



Material::Material(unsigned int id, unsigned int modelId): kId_(id), kModelId_(modelId), pkDiffuseTexture_(nullptr),
                   pkSpecularTexture_(nullptr), pkEmissiveTexture_(nullptr), pkNormalMapTexture_(nullptr),
	               kBlack_(vec3(0.0f)), shadingModel_(Material::ShadingModel::NO_SHADING), 
		           ambientColor_(vec3(0.0f)), diffuseColor_(vec3(0.0f)), specularColor_(vec3(0.0f)), emissiveColor_(vec3(0.0f)),
	               shininess_(0.0f), shininessStrength_(1.0f), opacity_(1.0f), twoSided_(false), wireframe_(false) {
		
	//cout << "Material " << toString() << " created." << endl;
}



Material::~Material () {
	//cout << "Material " << toString() << " deleted." << endl;
}



void  Material::setShadingModel(Material::ShadingModel shadingModel) {
	shadingModel_ = shadingModel;
}



void Material::setAmbientColor(const vec3& kColor) {
	if (std::abs(kColor.r - 0.5f) <= 0.5f && std::abs(kColor.g - 0.5f) <= 0.5f && std::abs(kColor.b - 0.5f) <= 0.5f)
		ambientColor_ = kColor;
	else throw runtime_error("Material.setAmbientColor|Invalid ambient color value.");
}



void Material::setDiffuseColor(const vec3& kColor) {
	if (std::abs(kColor.r - 0.5f) <= 0.5f && std::abs(kColor.g - 0.5f) <= 0.5f && std::abs(kColor.b - 0.5f) <= 0.5f)
		diffuseColor_ = kColor;
	else throw runtime_error("Material.setDiffuseColor|Invalid diffuse color value.");
}



void Material::setSpecularColor(const vec3& kColor) {
	if (std::abs(kColor.r - 0.5f) <= 0.5f && std::abs(kColor.g - 0.5f) <= 0.5f && std::abs(kColor.b - 0.5f) <= 0.5f)
		specularColor_ = kColor;
	else throw runtime_error("Material.setSpecularColor|Invalid specular color value.");
}



void Material::setEmissiveColor(const vec3& kColor) {
	if (std::abs(kColor.r - 0.5f) <= 0.5f && std::abs(kColor.g - 0.5f) <= 0.5f && std::abs(kColor.b - 0.5f) <= 0.5f)
		emissiveColor_ = kColor;
	else throw runtime_error("Material.setEmissiveColor|Invalid emissive color value.");
}



void Material::setDiffuseTexture(const ColorTexture* pkTexture) {
	if (pkTexture) pkDiffuseTexture_ = pkTexture;
	else pkDiffuseTexture_ = nullptr;
}



void Material::setSpecularTexture(const ColorTexture* pkTexture) {
	if (pkTexture) pkSpecularTexture_ = pkTexture;
	else pkSpecularTexture_ = nullptr;
}



void Material::setEmissiveTexture(const ColorTexture* pkTexture) {
	if (pkTexture) pkEmissiveTexture_ = pkTexture;
	else pkEmissiveTexture_ = nullptr;
}



void Material::setNormalMapTexture(const ColorTexture* pkTexture) {
	if (pkTexture) pkNormalMapTexture_ = pkTexture;
	else pkNormalMapTexture_ = nullptr;
}



void Material::setShininess(float shininess) {
	if (shininess >= 0.0f && shininess <= Material::getMaxShininess_())
		shininess_ = shininess;
	else throw runtime_error("Material.setShininess|Invalid shininess value.");
}



void Material::setShininessStrength(float strength) {
	if (strength >= 0.0f && strength <= 1.0f)
		shininessStrength_ = strength;
	else throw runtime_error("Material.setShininessStrength|Invalid strength value.");
}



void Material::setOpacity(float opacity) {
	if (opacity >= 0.0f && opacity <= 1.0f)
		opacity_ = opacity;	
	else throw runtime_error("Material.setOpacity|Invalid opacity value.");
}



void Material::setTwoSided(bool twoSided) {
	twoSided_ = twoSided;
}



void Material::setWireframe(bool wireframe) {
	wireframe_ = wireframe;
}



Material::ShadingModel Material::getShadingModel() const {
	return shadingModel_;
}



const vec3* Material::getAmbientColor() const {
	switch (shadingModel_) {
	case Material::ShadingModel::NO_SHADING:
	default:
		return &kBlack_;
	case Material::ShadingModel::GOURAUD:
	case Material::ShadingModel::PHONG:
		return &ambientColor_;
	}
}



const vec3* Material::getDiffuseColor() const {
	return &diffuseColor_;
}



const vec3* Material::getSpecularColor() const {
	switch (shadingModel_) {
	case Material::ShadingModel::NO_SHADING:
	case Material::ShadingModel::GOURAUD:
	default:
		return &kBlack_;
	case Material::ShadingModel::PHONG:
		return (shininess_ > 0.0f) ? &specularColor_ : &kBlack_;
	}
}



const vec3* Material::getEmissiveColor() const {
	switch (shadingModel_) {
	case Material::ShadingModel::NO_SHADING:
	default:
		return &kBlack_;
	case Material::ShadingModel::GOURAUD:
	case Material::ShadingModel::PHONG:
		return &emissiveColor_;
	}
}



const ColorTexture* Material::getDiffuseTexture() const {
	return pkDiffuseTexture_;
}



const ColorTexture* Material::getSpecularTexture() const {
	switch (shadingModel_) {
	case Material::ShadingModel::NO_SHADING:
	case Material::ShadingModel::GOURAUD:
	default:
		return nullptr;
	case Material::ShadingModel::PHONG:
		return (shininess_ > 0.0f) ? pkSpecularTexture_ : nullptr;
	}
}



const ColorTexture* Material::getEmissiveTexture() const {
	switch (shadingModel_) {
	case Material::ShadingModel::NO_SHADING:
	default:
		return nullptr;
	case Material::ShadingModel::GOURAUD:
	case Material::ShadingModel::PHONG:
		return pkEmissiveTexture_;
	}
}



const ColorTexture* Material::getNormalMapTexture() const {
	switch (shadingModel_) {
	case Material::ShadingModel::NO_SHADING:
	case Material::ShadingModel::GOURAUD:
	default:
		return nullptr;
	case Material::ShadingModel::PHONG:
		return pkNormalMapTexture_;
	}
}



float Material::getShininess() const {
	switch (shadingModel_) {
	case Material::ShadingModel::NO_SHADING:
	case Material::ShadingModel::GOURAUD:
	default:
		return 0.0f;
	case Material::ShadingModel::PHONG:
		return shininess_;
	}
}



float Material::getShininessStrength() const {
	return shininessStrength_;
}



float Material::getOpacity() const {
	return opacity_;
}



bool Material::isTwoSided() const {
	return twoSided_;
}



bool Material::isWireframe() const {
	return wireframe_;
}



bool Material::isTransparent() const {
	return (opacity_ < 1.0f) || (pkDiffuseTexture_ && pkDiffuseTexture_->isTransparent());
}



unsigned int Material::getId() const {
	return kId_;
}



unsigned int Material::getModelId() const {
	return kModelId_;
}



string Material::toString() const {
	return to_string(kModelId_) + "_" + to_string(kId_);
}
