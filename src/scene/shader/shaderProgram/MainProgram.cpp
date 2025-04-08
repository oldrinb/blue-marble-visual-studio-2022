/**
 * Author: Oldrin Barbulescu
 * Last modified: Feb 3, 2025
 **/

#include "MainProgram.h"



MainProgram::MainProgram() : BaseProgram(), shdLight_(), shdMaterial_(), shdScene_(),
                             shdModelViewMatrix_(-1), shdModelViewProjectionMatrix_(-1), shdNormalMatrix_(-1),
	                         shdHasDiffuseTexture_(-1), shdHasSpecularTexture_(-1), shdHasEmissiveTexture_(-1), 
	                         shdHasNormalMapTexture_(-1), shdDiffuseTexSampler_(-1), shdSpecularTexSampler_(-1),
	                         shdEmissiveTexSampler_(-1), shdNormalMapTexSampler_(-1), shdDiffuseCompressed_(-1),
	                         shdSpecularCompressed_(-1), shdEmissiveCompressed_(-1), shdNormalMapCompressed_(-1) {
	//cout << "Main program created." << endl;
}



MainProgram::~MainProgram() {
	//cout << "Main program deleted." << endl;
}



void MainProgram::link(MainProgram::ProgramMode programMode, list<GLuint>& rVertexShaderList, list<GLuint>& rFragmentShaderList) {
	try {
		bindAttribLocations_(programMode);
		bindOutputLocations_(programMode);

		pProgram_->link(rVertexShaderList, rFragmentShaderList);

		queryUniformLocations_(programMode);
	}
	catch (const exception& kException) {
		throw runtime_error("MainProgram.link > " + string(kException.what()));
	}
}



void MainProgram::setAttribPointers(Face* pMesh) const {
	pMesh->setAttribPointers(pProgram_->getId(), BaseProgram::getVerticesAttribLocation_(),
		                     BaseProgram::getTexCoordsAttribLocation_(), BaseProgram::getNormalsAttribLocation_(),
		                     BaseProgram::getTangentsAttribLocation_(), BaseProgram::getBitangentsAttribLocation_());
}



void MainProgram::start() const {
	if (pProgram_->isLinked())
		BaseProgram::start();
	else throw runtime_error("MainProgram.start|Main program " + to_string(pProgram_->getId()) + " not linked.");
}



void MainProgram::render(const Face* pkMesh) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled())
			try {
				pkMesh->render(pProgram_->getId());
			}
			catch (const exception& kException) {
				throw runtime_error("MainProgram.render > " + string(kException.what()));
			}
		else throw runtime_error("MainProgram.render|Main program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("MainProgram.render|Main program " + to_string(pProgram_->getId()) + " not linked.");
}



void MainProgram::setAmbientLightOn(GLuint on) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled()) {
			if (shdScene_.ambientLightOn != -1)
				pProgram_->setUniformui(shdScene_.ambientLightOn, on);
		}
		else throw runtime_error("MainProgram.setAmbientLightOn|Main program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("MainProgram.setAmbientLightOn|Main program " + to_string(pProgram_->getId()) + " not linked.");
}



void MainProgram::setDiffuseLightOn(GLuint on) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled()) {
			if (shdScene_.diffuseLightOn != -1)
				pProgram_->setUniformui(shdScene_.diffuseLightOn, on);
		}
		else throw runtime_error("MainProgram.setDiffuseLightOn|Main program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("MainProgram.setDiffuseLightOn|Main program " + to_string(pProgram_->getId()) + " not linked.");
}



void MainProgram::setSpecularLightOn(GLuint on) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled()) {
			if (shdScene_.specularLightOn != -1)
				pProgram_->setUniformui(shdScene_.specularLightOn, on);
		}
		else throw 
			runtime_error("MainProgram.setSpecularLightOn|Main program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("MainProgram.setSpecularLightOn|Main program " + to_string(pProgram_->getId()) + " not linked.");
}



void MainProgram::setEmissiveLightOn(GLuint on) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled()) {
			if (shdScene_.emissiveLightOn != -1)
				pProgram_->setUniformui(shdScene_.emissiveLightOn, on);
		}
		else throw
			runtime_error("MainProgram.setEmissiveLightOn|Main program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("MainProgram.setEmissiveLightOn|Main program " + to_string(pProgram_->getId()) + " not linked.");
}



void MainProgram::setLightEyeDirection(const GLfloat* pkDirection) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled()) {
			if (shdLight_.eDirection != -1)
				pProgram_->setUniformVector3f(shdLight_.eDirection, pkDirection);
		}
		else throw 
			runtime_error("MainProgram.setLightEyeDirection|Main program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("MainProgram.setLightEyeDirection|Main program " + to_string(pProgram_->getId()) + " not linked.");
}



void MainProgram::setLightColor(const GLfloat* pkAmbient, const GLfloat* pkDiffuse, const GLfloat* pkSpecular) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled()) {
			if (shdLight_.ambientColor != -1)
				pProgram_->setUniformVector3f(shdLight_.ambientColor, pkAmbient);

			if (shdLight_.diffuseColor != -1)
				pProgram_->setUniformVector3f(shdLight_.diffuseColor, pkDiffuse);

			if (shdLight_.specularColor != -1)
				pProgram_->setUniformVector3f(shdLight_.specularColor, pkSpecular);			
		}
		else throw runtime_error("MainProgram.setLightColor|Main program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("MainProgram.setLightColor|Main program " + to_string(pProgram_->getId()) + " not linked.");
}



void MainProgram::setModelViewMatrix(const GLfloat* pkMatrix) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled())
			pProgram_->setUniformMatrix4f(shdModelViewMatrix_, pkMatrix);
		
		else throw 
			runtime_error("MainProgram.setModelViewMatrix|Main program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("MainProgram.setModelViewMatrix|Main program " + to_string(pProgram_->getId()) + " not linked.");
}



void MainProgram::setModelViewProjectionMatrix(const GLfloat* pkMatrix) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled())
			pProgram_->setUniformMatrix4f(shdModelViewProjectionMatrix_, pkMatrix);
		
		else throw runtime_error
		          ("MainProgram.setModelViewProjectionMatrix|Main program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error
	              ("MainProgram.setModelViewProjectionMatrix|Main program " + to_string(pProgram_->getId()) + " not linked.");
}



void MainProgram::setNormalMatrix(const GLfloat* pkMatrix) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled())
			pProgram_->setUniformMatrix4f(shdNormalMatrix_, pkMatrix);
		
		else throw runtime_error("MainProgram.setNormalMatrix|Main program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("MainProgram.setNormalMatrix|Main program " + to_string(pProgram_->getId()) + " not linked.");
}



void MainProgram::setMaterialShadingModel(GLuint shadingModel) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled()) {
			if (shdMaterial_.shadingModel != -1)
				pProgram_->setUniformui(shdMaterial_.shadingModel, shadingModel);
		}
		else throw 
			runtime_error("MainProgram.setMaterialShadingModel|Main program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("MainProgram.setMaterialShadingModel|Main program " + to_string(pProgram_->getId()) + " not linked.");
}



void MainProgram::setMaterialColor(const GLfloat* pkAmbient, const GLfloat* pkDiffuse, 
	                               const GLfloat* pkSpecular, const GLfloat* pkEmissive) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled()) {
			pProgram_->setUniformVector3f(shdMaterial_.diffuseColor, pkDiffuse);

			if (shdMaterial_.ambientColor != -1)
				pProgram_->setUniformVector3f(shdMaterial_.ambientColor, pkAmbient);

			if (shdMaterial_.specularColor != -1)
				pProgram_->setUniformVector3f(shdMaterial_.specularColor, pkSpecular);

			if (shdMaterial_.emissiveColor != -1)
				pProgram_->setUniformVector3f(shdMaterial_.emissiveColor, pkEmissive);
		}
		else throw runtime_error("MainProgram.setMaterialColor|Main program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("MainProgram.setMaterialColor|Main program " + to_string(pProgram_->getId()) + " not linked.");
}



void MainProgram::setMaterialShininess(GLfloat shininess) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled()) {
			if (shdMaterial_.shininess != -1)
				pProgram_->setUniformf(shdMaterial_.shininess, shininess);
		}
		else throw 
			runtime_error("MainProgram.setMaterialShininess|Main program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("MainProgram.setMaterialShininess|Main program " + to_string(pProgram_->getId()) + " not linked.");
}



void MainProgram::setMaterialOpacity(GLfloat opacity) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled())
			pProgram_->setUniformf(shdMaterial_.opacity, opacity);
		
		else throw 
			runtime_error("MainProgram.setMaterialOpacity|Main program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("MainProgram.setMaterialOpacity|Main program " + to_string(pProgram_->getId()) + " not linked.");
}



void MainProgram::setDiffuseTexParameters(GLuint hasTexture, GLuint compressed) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled()) {
			pProgram_->setUniformui(shdHasDiffuseTexture_, hasTexture);
			pProgram_->setUniformui(shdDiffuseCompressed_, compressed);
		}
		else throw 
			runtime_error("MainProgram.setDiffuseTexParameters|Main program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("MainProgram.setDiffuseTexParameters|Main program " + to_string(pProgram_->getId()) + " not linked.");
}



void MainProgram::setSpecularTexParameters(GLuint hasTexture, GLuint compressed) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled()) {
			pProgram_->setUniformui(shdHasSpecularTexture_, hasTexture);
			pProgram_->setUniformui(shdSpecularCompressed_, compressed);
		}
		else throw 
			runtime_error("MainProgram.setSpecularTexParameters|Main program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("MainProgram.setSpecularTexParameters|Main program " + to_string(pProgram_->getId()) + " not linked.");
}



void MainProgram::setEmissiveTexParameters(GLuint hasTexture, GLuint compressed) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled()) {
			pProgram_->setUniformui(shdHasEmissiveTexture_, hasTexture);
			pProgram_->setUniformui(shdEmissiveCompressed_, compressed);
		}
		else throw 
			runtime_error("MainProgram.setEmissiveTexParameters|Main program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("MainProgram.setEmissiveTexParameters|Main program " + to_string(pProgram_->getId()) + " not linked.");
}



void MainProgram::setNormalTexParameters(GLuint hasTexture, GLuint compressed) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled()) {
			if (shdHasNormalMapTexture_ != -1)
				pProgram_->setUniformui(shdHasNormalMapTexture_, hasTexture);

			if (shdNormalMapCompressed_ != -1)
				pProgram_->setUniformui(shdNormalMapCompressed_, compressed);
		}
		else throw 
			runtime_error("MainProgram.setNormalTexParameters|Main program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("MainProgram.setNormalTexParameters|Main program " + to_string(pProgram_->getId()) + " not linked.");
}



void MainProgram::setDiffuseTextureUnit(GLint textureUnit) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled())
			pProgram_->setUniformi(shdDiffuseTexSampler_, textureUnit);
		
		else throw 
			runtime_error("MainProgram.setDiffuseTextureUnit|Main program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("MainProgram.setDiffuseTextureUnit|Main program " + to_string(pProgram_->getId()) + " not linked.");
}



void MainProgram::setSpecularTextureUnit(GLint textureUnit) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled())
			pProgram_->setUniformi(shdSpecularTexSampler_, textureUnit);
		
		else throw 
			runtime_error("MainProgram.setSpecularTextureUnit|Main program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("MainProgram.setSpecularTextureUnit|Main program " + to_string(pProgram_->getId()) + " not linked.");
}



void MainProgram::setEmissiveTextureUnit(GLint textureUnit) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled())
			pProgram_->setUniformi(shdEmissiveTexSampler_, textureUnit);
		
		else throw 
			runtime_error("MainProgram.setEmissiveTextureUnit|Main program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("MainProgram.setEmissiveTextureUnit|Main program " + to_string(pProgram_->getId()) + " not linked.");
}



void MainProgram::setNormalMapTextureUnit(GLint textureUnit) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled()) {
			if (shdNormalMapTexSampler_ != -1)
				pProgram_->setUniformi(shdNormalMapTexSampler_, textureUnit);
		}
		else throw
			runtime_error("MainProgram.setNormalMapTextureUnit|Main program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("MainProgram.setNormalMapTextureUnit|Main program " + to_string(pProgram_->getId()) + " not linked.");
}



void MainProgram::bindAttribLocations_(MainProgram::ProgramMode programMode) {
	try {
		pProgram_->setAttribLocation("mPosition", BaseProgram::getVerticesAttribLocation_());
		pProgram_->setAttribLocation("vTexCoord", BaseProgram::getTexCoordsAttribLocation_());
		pProgram_->setAttribLocation("mNormal", BaseProgram::getNormalsAttribLocation_());

		switch (programMode) {
		case MainProgram::ProgramMode::NO_SHADING:
		case MainProgram::ProgramMode::FLAT:
		case MainProgram::ProgramMode::GOURAUD:
		case MainProgram::ProgramMode::PHONG:
			break;

		case MainProgram::ProgramMode::NORMAL_MAPPING:
			pProgram_->setAttribLocation("mTangent", BaseProgram::getTangentsAttribLocation_());
			pProgram_->setAttribLocation("mBitangent", BaseProgram::getBitangentsAttribLocation_());
		}
	}
	catch (const exception& kException) {
		throw runtime_error("MainProgram.bindAttribLocations_ > " + string(kException.what()));
	}
}



void MainProgram::bindOutputLocations_(MainProgram::ProgramMode) {
	try {
		pProgram_->setOutputLocation("outputColor", BaseProgram::getOutputFragLocation_());
	}
	catch (const exception& kException) {
		throw runtime_error("MainProgram.bindOutputLocations_ > " + string(kException.what()));
	}
}



void MainProgram::queryUniformLocations_(MainProgram::ProgramMode programMode) {
	try {
		shdLight_ = { -1, -1, -1, -1 };
		shdMaterial_ = { -1, -1, -1, -1, -1, -1, -1 };
		shdScene_ = { -1, -1, -1, -1 };

		shdHasNormalMapTexture_ = shdNormalMapTexSampler_ = shdNormalMapCompressed_ = -1;
		
		shdMaterial_.opacity = pProgram_->getUniformLocation("material.opacity");

		shdModelViewMatrix_ = pProgram_->getUniformLocation("modelViewMatrix");
		shdModelViewProjectionMatrix_ = pProgram_->getUniformLocation("modelViewProjectionMatrix");
		shdNormalMatrix_ = pProgram_->getUniformLocation("normalMatrix");

		shdHasDiffuseTexture_ = pProgram_->getUniformLocation("hasDiffuseTexture");
		shdHasSpecularTexture_ = pProgram_->getUniformLocation("hasSpecularTexture");
		shdHasEmissiveTexture_ = pProgram_->getUniformLocation("hasEmissiveTexture");

		shdDiffuseTexSampler_ = pProgram_->getUniformLocation("diffuseTexSampler");
		shdSpecularTexSampler_ = pProgram_->getUniformLocation("specularTexSampler");
		shdEmissiveTexSampler_ = pProgram_->getUniformLocation("emissiveTexSampler");

		shdDiffuseCompressed_ = pProgram_->getUniformLocation("diffuseCompressed");
		shdSpecularCompressed_ = pProgram_->getUniformLocation("specularCompressed");
		shdEmissiveCompressed_ = pProgram_->getUniformLocation("emissiveCompressed");

		switch (programMode) {
		case MainProgram::ProgramMode::NORMAL_MAPPING:
			shdHasNormalMapTexture_ = pProgram_->getUniformLocation("hasNormalMapTexture");
			shdNormalMapTexSampler_ = pProgram_->getUniformLocation("normalMapTexSampler");
			shdNormalMapCompressed_ = pProgram_->getUniformLocation("normalMapCompressed");

		case MainProgram::ProgramMode::PHONG:
		case MainProgram::ProgramMode::GOURAUD:
		case MainProgram::ProgramMode::FLAT:
			shdMaterial_.ambientColor = pProgram_->getUniformLocation("material.ambientColor");
			shdMaterial_.specularColor = pProgram_->getUniformLocation("material.specularColor");
			shdMaterial_.emissiveColor = pProgram_->getUniformLocation("material.emissiveColor");
			shdMaterial_.shininess = pProgram_->getUniformLocation("material.shininess");

			shdScene_.ambientLightOn = pProgram_->getUniformLocation("scene.ambientLightOn");
			shdScene_.diffuseLightOn = pProgram_->getUniformLocation("scene.diffuseLightOn");
			shdScene_.specularLightOn = pProgram_->getUniformLocation("scene.specularLightOn");
			shdScene_.emissiveLightOn = pProgram_->getUniformLocation("scene.emissiveLightOn");

			{
				string name = "light.eDirection";
				shdLight_.eDirection = pProgram_->getUniformLocation(name.c_str());
				name = "light.ambientColor";
				shdLight_.ambientColor = pProgram_->getUniformLocation(name.c_str());
				name = "light.diffuseColor";
				shdLight_.diffuseColor = pProgram_->getUniformLocation(name.c_str());
				name = "light.specularColor";
				shdLight_.specularColor = pProgram_->getUniformLocation(name.c_str());
			}

		case MainProgram::ProgramMode::NO_SHADING:
			shdMaterial_.shadingModel = pProgram_->getUniformLocation("material.shadingModel");
			shdMaterial_.diffuseColor = pProgram_->getUniformLocation("material.diffuseColor");
		}
	}
	catch (const exception& kException) {
		throw runtime_error("MainProgram.queryUniformLocations_ > " + string(kException.what()));
	}
}
