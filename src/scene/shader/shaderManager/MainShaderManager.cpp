/**
 * Author: Oldrin Barbulescu
 * Last modified: Feb 3, 2025
 **/

#include "MainShaderManager.h"



GLint MainShaderManager::getDiffuseTextureUnit() {
	return 0;
}



GLint MainShaderManager::getSpecularTextureUnit() {
	return 1;
}



GLint MainShaderManager::getEmissiveTextureUnit() {
	return 2;
}



GLint MainShaderManager::getNormalMapTextureUnit() {
	return 3;
}



MainShaderManager::MainShaderManager(const uvec2& kWindowSize): BaseShaderManager(kWindowSize),
                                     pNoShadingProgram_(nullptr), pFlatShadingProgram_(nullptr), pGouraudShadingProgram_(nullptr),
	                                 pPhongShadingProgram_(nullptr), pNormalMappingProgram_(nullptr) {
	try {
		pNoShadingProgram_ = new MainProgram();
		pFlatShadingProgram_ = new MainProgram();
		pGouraudShadingProgram_ = new MainProgram();
		pPhongShadingProgram_ = new MainProgram();
		pNormalMappingProgram_ = new MainProgram();
	}
	catch (const exception& kException) {
		if (pNoShadingProgram_) delete pNoShadingProgram_;
		if (pFlatShadingProgram_) delete pFlatShadingProgram_;
		if (pGouraudShadingProgram_) delete pGouraudShadingProgram_;
		if (pPhongShadingProgram_) delete pPhongShadingProgram_;
		if (pNormalMappingProgram_) delete pNormalMappingProgram_;

		throw runtime_error("MainShaderManager > " + string(kException.what()));
	}

	nPrograms_ = 5u;
	ppPrograms_ = new BaseProgram*[nPrograms_];
	ppPrograms_[0u] = pNoShadingProgram_;
	ppPrograms_[1u] = pFlatShadingProgram_;
	ppPrograms_[2u] = pGouraudShadingProgram_;
	ppPrograms_[3u] = pPhongShadingProgram_;
	ppPrograms_[4u] = pNormalMappingProgram_;

	pCurrentProgram_ = pNoShadingProgram_;

	//cout << "Main shader manager created." << endl;
}



MainShaderManager::~MainShaderManager() {
	delete pNoShadingProgram_;
	delete pFlatShadingProgram_;
	delete pGouraudShadingProgram_;
	delete pPhongShadingProgram_;
	delete pNormalMappingProgram_;

	//cout << "Main shader manager deleted." << endl;
}



void MainShaderManager::addShaderSourceCode(MainProgram::ProgramMode programMode, const string& kPath,
	                                        const list<string>& kVertexShaderFileList, const string& kVertexShaderHeader,
	                                        const list<string>& kFragmentShaderFileList, const string& kFragmentShaderHeader) {
	try {
		addVertexShaderSourceCode(programMode, kPath, kVertexShaderFileList, kVertexShaderHeader);
		addFragmentShaderSourceCode(programMode, kPath, kFragmentShaderFileList, kFragmentShaderHeader);
	}
	catch (const exception& kException) {
		throw runtime_error("MainShaderManager.addShaderSourceCode > " + string(kException.what()));
	}
}



void MainShaderManager::addVertexShaderSourceCode(MainProgram::ProgramMode programMode, const string& kPath,
	                                              const list<string>& kFileList, const string& kHeader) {
	try {
		BaseShaderManager::addVertexShaderSourceCode_(static_cast<BaseProgram::ProgramMode>(programMode),
			                                          kPath, kFileList, kHeader);
	}
	catch (const exception& kException) {
		throw runtime_error("MainShaderManager.addVertexShaderSourceCode > " + string(kException.what()));
	}
}



void MainShaderManager::addFragmentShaderSourceCode(MainProgram::ProgramMode programMode, const string& kPath,
	                                                const list<string>& kFileList, const string& kHeader) {
	try {
		BaseShaderManager::addFragmentShaderSourceCode_(static_cast<BaseProgram::ProgramMode>(programMode),
			                                            kPath, kFileList, kHeader);
	}
	catch (const exception& kException) {
		throw runtime_error("MainShaderManager.addFragmentShaderSourceCode > " + string(kException.what()));
	}
}



void MainShaderManager::linkProgram(MainProgram::ProgramMode programMode) {
	try {
		list<GLuint> vertexShaderList, fragmentShaderList;
		BaseShaderManager::linkProgram_(static_cast<BaseProgram::ProgramMode>(programMode), vertexShaderList, fragmentShaderList);

		switch (programMode) {
		case MainProgram::ProgramMode::NO_SHADING:
			pNoShadingProgram_->link(programMode, vertexShaderList, fragmentShaderList);
			pCurrentProgram_ = pNoShadingProgram_;
			break;
		case MainProgram::ProgramMode::FLAT:
			pFlatShadingProgram_->link(programMode, vertexShaderList, fragmentShaderList);
			pCurrentProgram_ = pFlatShadingProgram_;
			break;
		case MainProgram::ProgramMode::GOURAUD:
			pGouraudShadingProgram_->link(programMode, vertexShaderList, fragmentShaderList);
			pCurrentProgram_ = pGouraudShadingProgram_;
			break;
		case MainProgram::ProgramMode::PHONG:
			pPhongShadingProgram_->link(programMode, vertexShaderList, fragmentShaderList);
			pCurrentProgram_ = pPhongShadingProgram_;
			break;
		case MainProgram::ProgramMode::NORMAL_MAPPING:
			pNormalMappingProgram_->link(programMode, vertexShaderList, fragmentShaderList);
			pCurrentProgram_ = pNormalMappingProgram_;
			break;
		default:
			return;
		}

		MainProgram* pProgram = static_cast<MainProgram*>(pCurrentProgram_);

		pProgram->start();

		pProgram->setDiffuseTextureUnit(MainShaderManager::getDiffuseTextureUnit());
		pProgram->setSpecularTextureUnit(MainShaderManager::getSpecularTextureUnit());
		pProgram->setEmissiveTextureUnit(MainShaderManager::getEmissiveTextureUnit());
		pProgram->setNormalMapTextureUnit(MainShaderManager::getNormalMapTextureUnit());

		pProgram->stop();
	}
	catch (const exception& kException) {
		throw runtime_error("MainShaderManager.linkProgram > " + string(kException.what()));
	}
}



void MainShaderManager::setAttribPointers(Face* pMesh) const {
	for (unsigned int i = 0u; i < nPrograms_; i++) {
		MainProgram* pProgram = static_cast<MainProgram*>(ppPrograms_[i]);
		pProgram->setAttribPointers(pMesh);
	}
}



void MainShaderManager::setShadingParameters(bool ambientLightOn, bool diffuseLightOn, 
	                                         bool specularLightOn, bool emissiveLightOn) const {
	try {
		for (unsigned int i = 0u; i < nPrograms_; i++) {
			MainProgram* pProgram = static_cast<MainProgram*>(ppPrograms_[i]);

			pProgram->start();
			pProgram->setAmbientLightOn(static_cast<GLuint>(ambientLightOn));
			pProgram->setDiffuseLightOn(static_cast<GLuint>(diffuseLightOn));
			pProgram->setSpecularLightOn(static_cast<GLuint>(specularLightOn));
			pProgram->setEmissiveLightOn(static_cast<GLuint>(emissiveLightOn));
			pProgram->stop();
		}
	}
	catch (const exception& kException) {
		throw runtime_error("MainShaderManager.setShadingParameters > " + string(kException.what()));
	}
}



void MainShaderManager::setLightEyeDirection(const vec3& kDirection) const {
	try {
		for (unsigned int i = 0u; i < nPrograms_; i++) {
			MainProgram* pProgram = static_cast<MainProgram*>(ppPrograms_[i]);

			pProgram->start();
			pProgram->setLightEyeDirection(value_ptr(kDirection));
			pProgram->stop();
		}
	}
	catch (const exception& kException) {
		throw runtime_error("MainShaderManager.setLightEyeDirection > " + string(kException.what()));
	}
}



void MainShaderManager::setLightParameters(const vec3& kAmbientColor,
	                                       const vec3& kDiffuseColor, const vec3& kSpecularColor) const {
	try {
		for (unsigned int i = 0u; i < nPrograms_; i++) {
			MainProgram* pProgram = static_cast<MainProgram*>(ppPrograms_[i]);

			pProgram->start();
			pProgram->setLightColor(value_ptr(kAmbientColor), value_ptr(kDiffuseColor), value_ptr(kSpecularColor));
			pProgram->stop();
		}
	}
	catch (const exception& kException) {
		throw runtime_error("MainShaderManager.setLightParameters > " + string(kException.what()));
	}
}



void MainShaderManager::startProgram(MainProgram::ProgramMode programMode) {
	switch (programMode) {
	case MainProgram::ProgramMode::NO_SHADING:
		pCurrentProgram_ = pNoShadingProgram_;
		break;
	case MainProgram::ProgramMode::FLAT:
		pCurrentProgram_ = pFlatShadingProgram_;
		break;
	case MainProgram::ProgramMode::GOURAUD:
		pCurrentProgram_ = pGouraudShadingProgram_;
		break;
	case MainProgram::ProgramMode::PHONG:
		pCurrentProgram_ = pPhongShadingProgram_;
		break;
	case MainProgram::ProgramMode::NORMAL_MAPPING:
		pCurrentProgram_ = pNormalMappingProgram_;
		break;
	default:
		throw runtime_error("MainShaderManager.startProgram|Invalid program mode value.");
	}

	try {
		pCurrentProgram_->start();
	}
	catch (const exception& kException) {
		throw runtime_error("MainShaderManager.startProgram > " + string(kException.what()));
	}
}



void MainShaderManager::render(const Face* pkMesh) const {
	try {
		MainProgram* pProgram = static_cast<MainProgram*>(pCurrentProgram_);
		pProgram->render(pkMesh);
	}
	catch (const exception& kException) {
		throw runtime_error("MainShaderManager.render > " + string(kException.what()));
	}
}



void MainShaderManager::setModelViewMatrix(const mat4& kMatrix) const {
	try {
		MainProgram* pProgram = static_cast<MainProgram*>(pCurrentProgram_);
		pProgram->setModelViewMatrix(value_ptr(kMatrix));
	}
	catch (const exception& kException) {
		throw runtime_error("MainShaderManager.setModelViewMatrix > " + string(kException.what()));
	}
}



void MainShaderManager::setModelViewProjectionMatrix(const mat4& kMatrix) const {
	try {
		MainProgram* pProgram = static_cast<MainProgram*>(pCurrentProgram_);
		pProgram->setModelViewProjectionMatrix(value_ptr(kMatrix));
	}
	catch (const exception& kException) {
		throw runtime_error("MainShaderManager.setModelViewProjectionMatrix > " + string(kException.what()));
	}
}



void MainShaderManager::setNormalMatrix(const mat4& kMatrix) const {
	try {
		MainProgram* pProgram = static_cast<MainProgram*>(pCurrentProgram_);
		pProgram->setNormalMatrix(value_ptr(kMatrix));
	}
	catch (const exception& kException) {
		throw runtime_error("MainShaderManager.setNormalMatrix > " + string(kException.what()));
	}
}



void MainShaderManager::setMaterialParameters(Material::ShadingModel shadingModel) const {
	try {
		MainProgram* pProgram = static_cast<MainProgram*>(pCurrentProgram_);
		pProgram->setMaterialShadingModel(static_cast<GLuint>(shadingModel));
	}
	catch (const exception& kException) {
		throw runtime_error("MainShaderManager.setMaterialParameters > " + string(kException.what()));
	}
}



void MainShaderManager::setMaterialParameters(const vec3& kAmbientColor, const vec3& kDiffuseColor,
	                                          const vec3& kSpecularColor, const vec3& kEmissiveColor) const {
	try {
		MainProgram* pProgram = static_cast<MainProgram*>(pCurrentProgram_);

		pProgram->setMaterialColor(value_ptr(kAmbientColor), value_ptr(kDiffuseColor), value_ptr(kSpecularColor),
			                       value_ptr(kEmissiveColor));
	}
	catch (const exception& kException) {
		throw runtime_error("MainShaderManager.setMaterialParameters > " + string(kException.what()));
	}
}



void MainShaderManager::setMaterialParameters(float shininess, float opacity) const {
	try {
		MainProgram* pProgram = static_cast<MainProgram*>(pCurrentProgram_);

		pProgram->setMaterialShininess(shininess);
		pProgram->setMaterialOpacity(opacity);	
	}
	catch (const exception& kException) {
		throw runtime_error("MainShaderManager.setMaterialParameters > " + string(kException.what()));
	}
}



void MainShaderManager::setDiffuseTexParameters(bool hasTexture, bool compressed) const {
	try {
		MainProgram* pProgram = static_cast<MainProgram*>(pCurrentProgram_);
		pProgram->setDiffuseTexParameters(static_cast<GLuint>(hasTexture), static_cast<GLuint>(compressed));
	}
	catch (const exception& kException) {
		throw runtime_error("MainShaderManager.setDiffuseTexParameters > " + string(kException.what()));
	}
}



void MainShaderManager::setSpecularTexParameters(bool hasTexture, bool compressed) const {
	try {
		MainProgram* pProgram = static_cast<MainProgram*>(pCurrentProgram_);
		pProgram->setSpecularTexParameters(static_cast<GLuint>(hasTexture), static_cast<GLuint>(compressed));
	}
	catch (const exception& kException) {
		throw runtime_error("MainShaderManager.setSpecularTexParameter > " + string(kException.what()));
	}
}



void MainShaderManager::setEmissiveTexParameters(bool hasTexture, bool compressed) const {
	try {
		MainProgram* pProgram = static_cast<MainProgram*>(pCurrentProgram_);
		pProgram->setEmissiveTexParameters(static_cast<GLuint>(hasTexture), static_cast<GLuint>(compressed));
	}
	catch (const exception& kException) {
		throw runtime_error("MainShaderManager.setEmissiveTexParameters > " + string(kException.what()));
	}
}



void MainShaderManager::setNormalTexParameters(bool hasTexture, bool compressed) const {
	try {
		MainProgram* pProgram = static_cast<MainProgram*>(pCurrentProgram_);
		pProgram->setNormalTexParameters(static_cast<GLuint>(hasTexture), static_cast<GLuint>(compressed));
	}
	catch (const exception& kException) {
		throw runtime_error("MainShaderManager.setNormalTexParameters > " + string(kException.what()));
	}
}
