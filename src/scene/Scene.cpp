/**
 * Author: Oldrin Barbulescu
 * Last modified: Feb 5, 2025
 **/

#include "Scene.h"



const float Scene::kLengthEpsilon_ = 0.01f;



float Scene::getMaxSceneRadius_() {
	return 40.0f;
}



float Scene::getMaxVerticalRotationAngle_() {
	return 45.0f;
}



Scene::Scene(const uvec2& kWindowSize):
	         perspectiveCameras_(), cameras_(), pActiveCamera_(nullptr),
	         directionalLights_(), lights_(), materials_(), meshes_(),
	         diffuseTextures_(), specularTextures_(), emissiveTextures_(), normalMapTextures_(),
	         pMainShaderManager_(nullptr), pInfo_(nullptr),
	         rotationMatrix_(mat4(1.0f)), cursorRotationMatrix_(mat4(1.0f)), rotationAngle_(0.0f), rotationSpeed_(0.0f),
	         cursorRotationAngleX_(0.0f), cursorRotationAngleY_(0.0f), isRotating_(false),	         
	         lastStartTime_(0.0), lastStopTime_(0.0), lastResetTime_(0.0),
	         shadingModel_(Scene::ShadingModel::PHONG),
	         hasSolid_(false), hasWireframe_(false), isSolid_(false), isWireframe_(false),
	         hasAmbientColor_(false), hasDiffuseColor_(false), hasSpecularColor_(false), hasEmissiveColor_(false),
	         ambientOn_(false), diffuseOn_(false), specularOn_(false), emissiveOn_(false),
	         hasNormalMapping_(false), isNormalMapping_(false),
	         windowSize_(kWindowSize), aspectRatio_(1.0f) {

	if (kWindowSize.x == 0u || kWindowSize.y == 0u)
		throw runtime_error("Scene|Invalid window size value.");
	aspectRatio_ = static_cast<float>(kWindowSize.x) / static_cast<float>(kWindowSize.y);

	try {
		pMainShaderManager_ = new MainShaderManager(kWindowSize);
		pInfo_ = new Info(kWindowSize);
	}
	catch (const exception& kException) {
		if (pMainShaderManager_) delete pMainShaderManager_;
		if (pInfo_) delete pInfo_;

		throw runtime_error("Scene > " + string(kException.what()));
	}

	cout << "Scene window size = " << kWindowSize.x << " x " << kWindowSize.y << " created." << endl << endl;
}



Scene::~Scene() {
	delete pMainShaderManager_;
	delete pInfo_;

	cout << "Scene deleted." << endl;
}



void Scene::addShaderSourceCode(MainProgram::ProgramMode programMode, const string& kPath,
	                            const list<string>& kVertexShaderFileList, const string& kVertexShaderHeader,
	                            const list<string>& kFragmentShaderFileList, const string& kFragmentShaderHeader) const {
	try {
		pMainShaderManager_->addShaderSourceCode(programMode, kPath, kVertexShaderFileList, kVertexShaderHeader,
			                                                         kFragmentShaderFileList, kFragmentShaderHeader);
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.addShaderSourceCode > " + string(kException.what()));
	}	
}



void Scene::addVertexShaderSourceCode(MainProgram::ProgramMode programMode, const string& kPath,
	                                  const list<string>& kFileList, const string& kHeader) const {
	try {
		pMainShaderManager_->addVertexShaderSourceCode(programMode, kPath, kFileList, kHeader);
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.addVertexShaderSourceCode > " + string(kException.what()));
	}
}



void Scene::addFragmentShaderSourceCode(MainProgram::ProgramMode programMode, const string& kPath,
	                                    const list<string>& kFileList, const string& kHeader) const {
	try {
		pMainShaderManager_->addFragmentShaderSourceCode(programMode, kPath, kFileList, kHeader);
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.addFragmentShaderSourceCode > " + string(kException.what()));
	}	
}



void Scene::addShaderSourceCode(Text2dProgram::ProgramMode programMode, const string& kPath,
	                            const list<string>& kVertexShaderFileList, const string& kVertexShaderHeader,
	                            const list<string>& kFragmentShaderFileList, const string& kFragmentShaderHeader) const {
	try {
		pInfo_->addShaderSourceCode(programMode, kPath, kVertexShaderFileList, kVertexShaderHeader,
			                                            kFragmentShaderFileList, kFragmentShaderHeader);
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.addShaderSourceCode > " + string(kException.what()));
	}
}



void Scene::addVertexShaderSourceCode(Text2dProgram::ProgramMode programMode, const string& kPath,
	                                  const list<string>& kFileList, const string& kHeader) const {
	try {
		pInfo_->addVertexShaderSourceCode(programMode, kPath, kFileList, kHeader);
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.addVertexShaderSourceCode > " + string(kException.what()));
	}
}



void Scene::addFragmentShaderSourceCode(Text2dProgram::ProgramMode programMode, const string& kPath,
	                                    const list<string>& kFileList, const string& kHeader) const {
	try {
		pInfo_->addFragmentShaderSourceCode(programMode, kPath, kFileList, kHeader);
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.addFragmentShaderSourceCode > " + string(kException.what()));
	}
}



void  Scene::compileShaders() const {
	try {
		pMainShaderManager_->compileShaders();

		pInfo_->compileShaders();

		pMainShaderManager_->linkProgram(MainProgram::ProgramMode::NO_SHADING);
		pMainShaderManager_->linkProgram(MainProgram::ProgramMode::FLAT);
		pMainShaderManager_->linkProgram(MainProgram::ProgramMode::GOURAUD);
		pMainShaderManager_->linkProgram(MainProgram::ProgramMode::PHONG);
		pMainShaderManager_->linkProgram(MainProgram::ProgramMode::NORMAL_MAPPING);		
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.compileShaders > " + string(kException.what()));
	}
}



void Scene::setText2DTexture(const string& kFilePath, const string& kFileName) const {
	try {
		pInfo_->setText2DTexture(kFilePath, kFileName);
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.setText2DTexture > " + string(kException.what()));
	}
}



void Scene::import3DModel(const string& kFilePath, const string& kFileName, unsigned int postProcessSteps) {
	try {
		Model3D model3D(kFilePath, kFileName, postProcessSteps, 0, false, false);

		model3D.loadMaterials(materials_);
		model3D.loadTextures(diffuseTextures_, specularTextures_, emissiveTextures_, normalMapTextures_);
		model3D.loadMeshes(meshes_);
		model3D.loadTransformations();
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.import3DModel > " + string(kException.what()));
	}

	if (meshes_.size() == 0u)
		throw runtime_error("Scene.import3DModel|There should be at least one mesh in the scene.");
}



void Scene::loadBufferData() {
	try {
		for (Mesh& iMesh : meshes_) {
			iMesh.updateVertexBuffer();
			pMainShaderManager_->setAttribPointers(&iMesh);
		}
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.loadBufferData > " + string(kException.what()));
	}
}



void Scene::translateMesh(int modelId, int meshId, const vec3& kDistance) {
	if (meshes_.size() == 0u)
		throw runtime_error("Scene.translateMesh|There should be at least one mesh in the scene.");

	if (modelId == -1 && meshId == -1)
		for (Mesh& iMesh : meshes_)
			try {
				iMesh.translate(kDistance);
			}
			catch (const exception& kException) {
				throw runtime_error("Scene.translateMesh > " + string(kException.what()));
			}
	else {
		Mesh* pMesh = getMesh_(static_cast<unsigned int>(modelId), static_cast<unsigned int>(meshId));
		if (pMesh)
			try {
				pMesh->translate(kDistance);
			}
			catch (const exception& kException) {
				throw runtime_error("Scene.translateMesh > " + string(kException.what()));
			}

		else throw runtime_error("Scene.translateMesh|Invalid model or mesh id value.");
	}
}



void Scene::scaleMesh(int modelId, int meshId, const vec3& kScale) {
	if (meshes_.size() == 0u)
		throw runtime_error("Scene.scaleMesh|There should be at least one mesh in the scene.");

	if (modelId == -1 && meshId == -1)
		for (Mesh& iMesh : meshes_)
			try {
				iMesh.scale(kScale);
			}
			catch (const exception& kException) {
				throw runtime_error("Scene.scaleMesh > " + string(kException.what()));
			}
	else {
		Mesh* pMesh = getMesh_(static_cast<unsigned int>(modelId), static_cast<unsigned int>(meshId));
		if (pMesh)
			try {
				pMesh->scale(kScale);
			}
			catch (const exception& kException) {
				throw runtime_error("Scene.scaleMesh > " + string(kException.what()));
			}

		else throw runtime_error("Scene.scaleMesh|Invalid model or mesh id value.");
	}
}



void Scene::rotateMesh(int modelId, int meshId, float angle, const vec3& kAxis) {
	if (meshes_.size() == 0u)
		throw runtime_error("Scene.rotateMesh|There should be at least one mesh in the scene.");

	if (modelId == -1 && meshId == -1)
		for (Mesh& iMesh : meshes_)
			try {
				iMesh.rotate(angle, kAxis);
			}
			catch (const exception& kException) {
				throw runtime_error("Scene.rotateMeshh > " + string(kException.what()));
			}
	else {
		Mesh* pMesh = getMesh_(static_cast<unsigned int>(modelId), static_cast<unsigned int>(meshId));
		if (pMesh)
			try {
				pMesh->rotate(angle, kAxis);
			}
			catch (const exception& kException) {
				throw runtime_error("Scene.rotateMesh > " + string(kException.what()));
			}

		else throw runtime_error("Scene.rotateMesh|Invalid model or mesh id value.");
	}
}



void Scene::setMeshWireframe(int modelId, int meshId, bool wireframe) {
	if (meshes_.size() == 0u)
		throw runtime_error("Scene.setMeshWireframe|There should be at least one mesh in the scene.");

	if (modelId == -1 && meshId == -1)
		for (Mesh& iMesh : meshes_)
			iMesh.setWireframe(wireframe);

	else {
		Mesh* pMesh = getMesh_(static_cast<unsigned int>(modelId), static_cast<unsigned int>(meshId));
		if (pMesh) pMesh->setWireframe(wireframe);
		else throw runtime_error("Scene.setMeshWireframe|Invalid model or mesh id value.");
	}
}




void Scene::setMeshRotationSpeed(int modelId, int meshId, float value) {
	if (meshes_.size() == 0u)
		throw runtime_error("Scene.setMeshRotationSpeed|There should be at least one mesh in the scene.");

	if (modelId == -1 && meshId == -1)
		for (Mesh& iMesh : meshes_)
			iMesh.setRotationSpeed(value);
	else {
		Mesh* pMesh = getMesh_(static_cast<unsigned int>(modelId), static_cast<unsigned int>(meshId));
		if (pMesh) pMesh->setRotationSpeed(value);
		else throw runtime_error("Scene.setMeshRotationSpeed|Invalid model or mesh id value.");
	}
}



void Scene::addCamera(const vec3& kPosition, const vec3& kLookAt, float fieldOfView) {
	try {
		addPerspectiveCamera_(kPosition, kLookAt, fieldOfView);
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.addPerspectiveCamera > " + string(kException.what()));
	}
}



void Scene::setLight(const vec3& kDirection, const vec3& kAmbientColor, const vec3& kDiffuseColor,
	                            const vec3& kSpecularColor) {
	try {
		setDirectionalLight_(kDirection, kAmbientColor, kDiffuseColor, kSpecularColor);
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.addDirectionalLight > " + string(kException.what()));
	}
}



void Scene::setRotationSpeed(float value) {
	rotationSpeed_ = value;
	isRotating_ = (value != 0.0f);
}



void Scene::initializeSceneParameters() {
	if (pActiveCamera_) throw runtime_error("Scene.initializeSceneParameters|Scene parameters already initialized.");
	if (cameras_.size() > 0u) pActiveCamera_ = getCamera_(0u);
	if (!pActiveCamera_) throw runtime_error("Scene.initializeSceneParameters|There should be at least one camera in the scene.");

	if (meshes_.size() == 0u)
		throw runtime_error("Scene.initializeSceneParameters|There should be at least one mesh in the scene.");

	try {
		initializeSceneParameters_();
		initializeShadingParameters_();
		initializeCameraParameters_();
		updateDisplayInfo_();
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.initializeSceneParameters > " + string(kException.what()));
	}
}



void Scene::setWindowSize(const uvec2& kSize) {
	if (kSize.x == 0u || kSize.y == 0u)
		throw runtime_error("Scene.setWindowSize|Invalid window size value.");

	windowSize_ = kSize;
	aspectRatio_ = static_cast<float>(kSize.x) / static_cast<float>(kSize.y);

	try {
		for (PerspectiveCamera& iCamera : perspectiveCameras_)
			iCamera.setAspectRatio(aspectRatio_);

		pInfo_->setWindowSize(kSize);
		updateDisplayInfo_();
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.setWindowSize > " + string(kException.what()));
	}

	//cout << "Scene window size = " << kSize.x << " x " << kSize.y << "." << endl;
}



void Scene::toggleDisplayInfo() const {
	if (pInfo_->isEnabled())
		try {
			if (pInfo_->isOn()) pInfo_->setOff();
			else pInfo_->setOn();

			updateDisplayInfo_();
		}
		catch (const exception& kException) {
			throw runtime_error("Scene.toggleDisplayInfo > " + string(kException.what()));
		}	
}



void Scene::toggleRotation(double currentTime) {
	if (rotationSpeed_ != 0.0f) {
		isRotating_ = !isRotating_;

		if (isRotating_) lastStartTime_ = currentTime;
		else lastStopTime_ += currentTime - lastStartTime_;

		try {
			updateDisplayInfo_();
		}
		catch (const exception& kException) {
			throw runtime_error("Scene.toggleRotation > " + string(kException.what()));
		}
	}
}



void Scene::resetRotation(double currentTime) {
	rotationAngle_ = 0.0f;
	updateRotationMatrix_();

	cursorRotationAngleX_ = 0.0f;
	cursorRotationAngleY_ = 0.0f;
	updateCursorRotationMatrix_();

	lastResetTime_ = lastStopTime_;
	if (isRotating_) lastResetTime_ += currentTime - lastStartTime_;

	try {
		updateDisplayInfo_();
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.resetRotation > " + string(kException.what()));
	}
}



void Scene::toggleWireframe() {
	if (hasSolid_) isSolid_ = !isSolid_;
	else isSolid_ = false;
	if (hasWireframe_) isWireframe_ = true;
	else isWireframe_ = !isWireframe_;

	try {
		updateDisplayInfo_();
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.toggleWireframe > " + string(kException.what()));
	}
}



void Scene::setShadingModel(Scene::ShadingModel shadingModel) {
	try {
		setShadingModel_(shadingModel);
		updateDisplayInfo_();
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.setShadingModel > " + string(kException.what()));
	}
}



void Scene::toggleAmbientLight() {
	try {
		toggleLightComponent_(BaseLight::LightComponent::AMBIENT);
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.toggleAmbientLight > " + string(kException.what()));
	}	
}



void Scene::toggleDiffuseLight() {
	try {
		toggleLightComponent_(BaseLight::LightComponent::DIFFUSE);
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.toggleDiffuseLight > " + string(kException.what()));
	}
	
}



void Scene::toggleSpecularLight() {
	try {
		toggleLightComponent_(BaseLight::LightComponent::SPECULAR);
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.toggleSpecularLight > " + string(kException.what()));
	}
	
}



void Scene::toggleEmissiveLight() {
	try {
		toggleLightComponent_(BaseLight::LightComponent::EMISSIVE);
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.toggleEmissiveLight > " + string(kException.what()));
	}	
}



void Scene::toggleNormalMapping() {
	if (hasNormalMapping_) {
		try {
			setNormalMapping_(!isNormalMapping_);
			updateDisplayInfo_();
		}
		catch (const exception& kException) {
			throw runtime_error("Scene.toggleNormalMapping > " + string(kException.what()));
		}
	}
}



void Scene::setActiveCamera(unsigned int id) {
	if (!pActiveCamera_) throw runtime_error("Scene.setActiveCamera|Scene parameters not initialized yet.");

	BaseCamera* pCamera = getCamera_(id);
	if (pCamera) {
		pActiveCamera_ = pCamera;

		try {
			updateLightInEyeSpace_();
			updateDisplayInfo_();
		}
		catch (const exception& kException) {
			throw runtime_error("Scene.setActiveCamera > " + string(kException.what()));
		}
	}
	else if (id < BaseCamera::MAX_NUMBER_OF_CAMERAS) {}
	else throw runtime_error("Scene.setActiveCamera|Invalid camera id value.");
}



void Scene::translateCameraRight(float distance) const {
	if (pActiveCamera_)
		try {
			pActiveCamera_->translateRight(distance);
			computeCameraFrustrum_(pActiveCamera_);

			updateLightInEyeSpace_();
			updateDisplayInfo_();
		}
		catch (const exception& kException) {
			throw runtime_error("Scene.translateCameraRight > " + string(kException.what()));
		}	
}



void Scene::translateCameraUp(float distance) const {
	if (pActiveCamera_)
		try {
			pActiveCamera_->translateUp(distance);
			computeCameraFrustrum_(pActiveCamera_);

			updateLightInEyeSpace_();
			updateDisplayInfo_();
		}
		catch (const exception& kException) {
			throw runtime_error("Scene.translateCameraUp > " + string(kException.what()));
		}	
}



void Scene::translateCameraForward(float distance) const {
	if (pActiveCamera_)
		try {
			pActiveCamera_->translateForward(distance);
			computeCameraFrustrum_(pActiveCamera_);

			updateLightInEyeSpace_();
			updateDisplayInfo_();
		}
		catch (const exception& kException) {
			throw runtime_error("Scene.translateCameraForward > " + string(kException.what()));
		}	
}



void Scene::translateCameraToCenter(float distance) const {
	if (pActiveCamera_)
		try {
			pActiveCamera_->translateToCenter(distance);
			computeCameraFrustrum_(pActiveCamera_);

			updateLightInEyeSpace_();
			updateDisplayInfo_();
		}
		catch (const exception& kException) {
			throw runtime_error("Scene.translateCameraToCenter > " + string(kException.what()));
		}	
}



void Scene::rotateCameraRight(float angle) const {
	if (pActiveCamera_)
		try {
			pActiveCamera_->rotateRight(angle);
			computeCameraFrustrum_(pActiveCamera_);

			updateLightInEyeSpace_();
			updateDisplayInfo_();
		}
		catch (const exception& kException) {
			throw runtime_error("Scene.rotateCameraRight > " + string(kException.what()));
		}	
}



void Scene::rotateCameraUp(float angle) const {
	if (pActiveCamera_)
		try {
			pActiveCamera_->rotateUp(angle);
			computeCameraFrustrum_(pActiveCamera_);

			updateLightInEyeSpace_();
			updateDisplayInfo_();
		}
		catch (const exception& kException) {
			throw runtime_error("Scene.rotateCameraUp > " + string(kException.what()));
		}	
}



void Scene::resetCamera() const {
	if (pActiveCamera_)
		try {
			pActiveCamera_->reset();
			computeCameraFrustrum_(pActiveCamera_);

			updateLightInEyeSpace_();
			updateDisplayInfo_();
		}
		catch (const exception& kException) {
			throw runtime_error("Scene.resetCamera > " + string(kException.what()));
		}	
}



void Scene::setCameraFieldOfView(float angle) const {
	if (pActiveCamera_)
		switch (pActiveCamera_->getType()) {
		case BaseCamera::CameraType::PERSPECTIVE:
			try {
				static_cast<PerspectiveCamera*>(pActiveCamera_)->setFieldOfView(angle);
			}
			catch (const exception&) {}

			try {
				updateDisplayInfo_();
			}
			catch (const exception& kException) {
				throw runtime_error("Scene.setCameraFieldOfView > " + string(kException.what()));
			}

		case BaseCamera::CameraType::ORTHOGRAPHIC:
			break;
		}
}



void Scene::changeCameraFieldOfView(float deltaAngle) const {
	if (pActiveCamera_) {
		float fieldOfView = 0.0f;

		switch (pActiveCamera_->getType()) {
		case BaseCamera::CameraType::PERSPECTIVE:
			fieldOfView = static_cast<PerspectiveCamera*>(pActiveCamera_)->getFieldOfView() + deltaAngle;

			try {
				setCameraFieldOfView(fieldOfView);
			}
			catch (const exception& kException) {
				throw runtime_error("Scene.changeCameraFieldOfView > " + string(kException.what()));
			}

		case BaseCamera::CameraType::ORTHOGRAPHIC:
			break;
		}
	}
}



void Scene::resetCameraFieldOfView() const {
	if (pActiveCamera_)
		switch (pActiveCamera_->getType()) {
		case BaseCamera::CameraType::PERSPECTIVE:
			try {
				static_cast<PerspectiveCamera*>(pActiveCamera_)->resetFieldOfView();

				updateDisplayInfo_();
			}
			catch (const exception& kException) {
				throw runtime_error("Scene.resetCameraFieldOfView > " + string(kException.what()));
			}

		case BaseCamera::CameraType::ORTHOGRAPHIC:
			break;
		}
}



void Scene::updateMouseMovement(float angleX, float angleY) {
	float result = cursorRotationAngleX_ + angleX;

	if (std::abs(result) < Scene::getMaxVerticalRotationAngle_())
		cursorRotationAngleX_ = result;

	cursorRotationAngleY_ += angleY;
	cursorRotationAngleY_ = 180.0f + std::remainder(cursorRotationAngleY_ - 180.0f, 360.0f);

	updateCursorRotationMatrix_();
}



void Scene::updateRotation(float deltaTime) {
	if (isRotating_) rotationAngle_ += deltaTime * rotationSpeed_;
	rotationAngle_ = 180.0f + std::remainder(rotationAngle_ - 180.0f, 360.0f);

	updateRotationMatrix_();

	if (deltaTime > 0.0f)
		for (Mesh& iMesh : meshes_)
			iMesh.updateRotation(deltaTime);
}



void Scene::render(double currentTime, unsigned int fps) {
	if (!pActiveCamera_) throw runtime_error("Scene.render|Scene parameters not initialized yet.");

	try {
		renderToScreen_(currentTime, fps);
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.render > " + string(kException.what()));
	}
}



void Scene::addPerspectiveCamera_(const vec3& kPosition, const vec3& kLookAt, float fieldOfView) {
	unsigned int id = static_cast<unsigned int>(cameras_.size());
	if (id >= BaseCamera::MAX_NUMBER_OF_CAMERAS)
		throw runtime_error("Scene.addCamera_|Maximum number of cameras already allocated.");

	BaseCamera* pCamera = nullptr;

	try {
		perspectiveCameras_.emplace_back(id, kPosition, kLookAt, fieldOfView, aspectRatio_);
		pCamera = static_cast<BaseCamera*> (&perspectiveCameras_.back());		
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.addCamera_ > " + string(kException.what()));
	}

	if (pCamera) cameras_.push_back(pCamera);
}



void Scene::setDirectionalLight_(const vec3& kDirection,
	                  const vec3& kAmbientColor, const vec3& kDiffuseColor, const vec3& kSpecularColor) {
	unsigned int id = static_cast<unsigned int>(lights_.size());
	if (id >= 1)
		throw runtime_error("Scene.addLight_|Maximum number of lights already allocated.");

	BaseLight* pLight = nullptr;
	bool on = true;

	try {
		directionalLights_.emplace_back(id, kDirection, kAmbientColor, kDiffuseColor, kSpecularColor, on);
		pLight = static_cast<BaseLight*> (&directionalLights_.back());		
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.addLight_ > " + string(kException.what()));
	}

	if (pLight) lights_.push_back(pLight);
}



void Scene::initializeSceneParameters_() {
	for (const Mesh& ikMesh : meshes_) {
		if (ikMesh.isWireframe()) {
			hasWireframe_ = true;
			isWireframe_ = true;
		}
		else {
			hasSolid_ = true;
			isSolid_ = true;
		}
	}

	try {
		computeBoundingBox_();
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.initializeSceneParameters_ > " + string(kException.what()));
	}	
}



void Scene::initializeShadingParameters_() {
	for (const Mesh& ikMesh : meshes_) {
		const Material* pkMaterial = ikMesh.getMaterial();
		if (pkMaterial) {
			if (lights_.size() > 0u) {
				vec3 color = *(pkMaterial->getAmbientColor());
				if (color.r + color.g + color.b > 0.0f) {
					hasAmbientColor_ = true;
					ambientOn_ = true;
				}

				color = *(pkMaterial->getDiffuseColor());
				if (color.r + color.g + color.b > 0.0f || pkMaterial->getDiffuseTexture()) {
					hasDiffuseColor_ = true;
					diffuseOn_ = true;
				}

				color = *(pkMaterial->getSpecularColor());
				if (color.r + color.g + color.b > 0.0f || pkMaterial->getSpecularTexture()) {
					hasSpecularColor_ = true;
					specularOn_ = true;
				}

				if (ikMesh.hasTangentsAndBitangents() && pkMaterial->getNormalMapTexture()) {
					hasNormalMapping_ = true;
					isNormalMapping_ = true;
				}
			}

			vec3 color = *(pkMaterial->getEmissiveColor());
			if (color.r + color.g + color.b > 0.0f || pkMaterial->getEmissiveTexture()) {
				hasEmissiveColor_ = true;
				emissiveOn_ = true;
			}
		}
	}

	try {
		if (isNormalMapping_) setNormalMapping_(isNormalMapping_);
		else setShadingModel_(shadingModel_);

		pMainShaderManager_->setShadingParameters(ambientOn_, diffuseOn_, specularOn_, emissiveOn_);

		initializeLightParameters_();
		updateLightInEyeSpace_();
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.initializeShadingParameters_ > " + string(kException.what()));
	}
}



void Scene::initializeLightParameters_() {
	try {
		for (const BaseLight* ikLight : lights_) {
			unsigned int id = ikLight->getId();
			pMainShaderManager_->setLightParameters(*(ikLight->getAmbientColor()), *(ikLight->getDiffuseColor()),
				                                    *(ikLight->getSpecularColor()));
		}
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.initializeLightParameters_ > " + string(kException.what()));
	}
}



void Scene::initializeCameraParameters_() {
	try {
		for (BaseCamera* iCamera : cameras_)
			computeCameraFrustrum_(iCamera);
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.initializeCameraParameters_ > " + string(kException.what()));
	}	
}



void Scene::computeBoundingBox_() {
	vec3 sceneMin = vec3(std::numeric_limits<float>::max());
	vec3 sceneMax = vec3(-std::numeric_limits<float>::max());
	vec3 sceneCenter = vec3(0.0f);

	float sceneRadius = 0.0f;

	try {
		for (const Mesh& ikMesh : meshes_) {
			for (unsigned int i = 0u; i < ikMesh.getNumVertices(); i++) {
				vec4 vertex = vec4(*(ikMesh.getVertex(i)), 1.0f);
				vertex = (*(ikMesh.getModelMatrix())) * vertex;

				if (vertex.x < sceneMin.x) sceneMin.x = vertex.x;
				if (vertex.y < sceneMin.y) sceneMin.y = vertex.y;
				if (vertex.z < sceneMin.z) sceneMin.z = vertex.z;

				if (vertex.x > sceneMax.x) sceneMax.x = vertex.x;
				if (vertex.y > sceneMax.y) sceneMax.y = vertex.y;
				if (vertex.z > sceneMax.z) sceneMax.z = vertex.z;
			}
		}

		sceneCenter.x = (sceneMin.x + sceneMax.x) / 2.0f;
		sceneCenter.y = (sceneMin.y + sceneMax.y) / 2.0f;
		sceneCenter.z = (sceneMin.z + sceneMax.z) / 2.0f;

		for (const Mesh& ikMesh : meshes_) {
			for (unsigned int i = 0u; i < ikMesh.getNumVertices(); i++) {
				vec4 vertex = vec4(*(ikMesh.getVertex(i)), 1.0f);
				vertex = (*(ikMesh.getModelMatrix())) * vertex;

				float distance = glm::distance(sceneCenter, vec3(vertex));
				if (sceneRadius < distance) sceneRadius = distance;
			}
		}
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.computeBoundingBox_ > " + string(kException.what()));
	}

	if (sceneRadius <= Scene::kLengthEpsilon_)
		throw runtime_error("Scene.computeBoundingBox_|Scene too small.");
	else if (sceneRadius > Scene::getMaxSceneRadius_())
		throw runtime_error("Scene.computeBoundingBox_|Scene too large.");

	try {
		float scaleFactor = 0.5f / sceneRadius;
		translateMesh(-1, -1, -sceneCenter);
		scaleMesh(-1, -1, vec3(scaleFactor, scaleFactor, scaleFactor));
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.computeBoundingBox_ > " + string(kException.what()));
	}
}



void Scene::computeCameraFrustrum_(BaseCamera* pCamera) const {
	float distToSceneCentre = glm::distance(vec3(0.0f), *(pCamera->getPosition()));

	if (distToSceneCentre > Scene::kLengthEpsilon_) {
		vec3 cameraToScene = glm::normalize(vec3(0.0f) - *(pCamera->getPosition()));
		vec3 cameraLookAt = glm::normalize(*(pCamera->getLookAt()) - *(pCamera->getPosition()));

		distToSceneCentre *= glm::dot(cameraToScene, cameraLookAt);
	}

	float nearPlane = std::max(0.0f, distToSceneCentre - 0.5f);		
	float farPlane = nearPlane + 1.0f;

	try {
		pCamera->setNearAndFarPlanes(nearPlane, farPlane);
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.computeCameraFrustrum_ > " + string(kException.what()));
	}
}



void Scene::setShadingModel_(Scene::ShadingModel shadingModel) {
	shadingModel_ = shadingModel;
	switch (shadingModel_) {
	case Scene::ShadingModel::NO_SHADING:
	case Scene::ShadingModel::FLAT:
	case Scene::ShadingModel::GOURAUD:
		isNormalMapping_ = false;
	case Scene::ShadingModel::PHONG:
		break;
	}
}



void Scene::setNormalMapping_(bool on) {
	isNormalMapping_ = on;
	if (on) shadingModel_ = Scene::ShadingModel::PHONG;
}



void Scene::toggleLightComponent_(BaseLight::LightComponent component) {
	switch (shadingModel_) {
	case Scene::ShadingModel::FLAT:
	case Scene::ShadingModel::GOURAUD:
	case Scene::ShadingModel::PHONG:
		switch (component) {
		case BaseLight::LightComponent::AMBIENT:
			if (hasAmbientColor_) ambientOn_ = !ambientOn_;
			break;
		case BaseLight::LightComponent::DIFFUSE:
			if (hasDiffuseColor_) diffuseOn_ = !diffuseOn_;
			break;
		case BaseLight::LightComponent::SPECULAR:
			if (hasSpecularColor_) specularOn_ = !specularOn_;
			break;
		case BaseLight::LightComponent::EMISSIVE:
			if (hasEmissiveColor_) emissiveOn_ = !emissiveOn_;
			break;
		default: return;
		}

		try {
			pMainShaderManager_->setShadingParameters(ambientOn_, diffuseOn_, specularOn_, emissiveOn_);
			updateDisplayInfo_();
		}
		catch (const exception& kException) {
			throw runtime_error("Scene.toggleLightComponent_ > " + string(kException.what()));
		}

	case Scene::ShadingModel::NO_SHADING:
		break;
	}
}



void Scene::updateRotationMatrix_() {
	vec3 yAxis = vec3(0.0f, 1.0f, 0.0f);
	rotationMatrix_ = glm::rotate(mat4(1.0f), glm::radians(rotationAngle_), yAxis);
}



void Scene::updateCursorRotationMatrix_() {
	if (pActiveCamera_) {
		vec3 xAxis = glm::inverse(mat3(*(pActiveCamera_->getViewMatrix()))) * vec3(1.0f, 0.0f, 0.0f);
		vec3 yAxis = vec3(0.0f, 1.0f, 0.0f);

		cursorRotationMatrix_ = glm::rotate(mat4(1.0f), glm::radians(cursorRotationAngleX_), xAxis) *
		                        glm::rotate(mat4(1.0f), glm::radians(cursorRotationAngleY_), yAxis);
	}
}



void Scene::updateLightInEyeSpace_() const {
	try {
		for (BaseLight* iLight : lights_)
			iLight->setCameraViewMatrix(*(pActiveCamera_->getViewMatrix()));

		for (const DirectionalLight& iLight : directionalLights_)
			pMainShaderManager_->setLightEyeDirection(*(iLight.getEyeDirection()));
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.updateLightInEyeSpace_ > " + string(kException.what()));
	}
}



void Scene::updateDisplayInfo_() const {
	try {
		updateMainText_();
		updateSceneText_();
		updateShadingText_();
		updateCameraText_();
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.updateDisplayInfo_ > " + string(kException.what()));
	}
}



void Scene::updateMainText_() const {
	try {
		bool hasRotation = (rotationSpeed_ != 0.0f);
		pInfo_->setMainText(hasRotation, isRotating_);
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.updateMainText_ > " + string(kException.what()));
	}
}



void Scene::updateSceneText_() const {
	try {
		pInfo_->setSceneText(hasSolid_, isSolid_, isWireframe_);
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.updateSceneText_ > " + string(kException.what()));
	}
}



void Scene::updateShadingText_() const {
	try {
		bool hasAmbientColor = false, hasDiffuseColor = false, hasSpecularColor = false, hasEmissiveColor = false;

		switch (shadingModel_) {
		case Scene::ShadingModel::FLAT:
		case Scene::ShadingModel::GOURAUD:
		case Scene::ShadingModel::PHONG:
			hasAmbientColor = hasAmbientColor_;
			hasDiffuseColor = hasDiffuseColor_;
			hasSpecularColor = hasSpecularColor_;
			hasEmissiveColor = hasEmissiveColor_;

		case Scene::ShadingModel::NO_SHADING:
			break;
		}

		pInfo_->setShadingText(shadingModel_ == Scene::ShadingModel::FLAT, shadingModel_ == Scene::ShadingModel::GOURAUD,
			                   shadingModel_ == Scene::ShadingModel::PHONG,
			                   hasAmbientColor, hasDiffuseColor, hasSpecularColor, hasEmissiveColor, ambientOn_, diffuseOn_,
			                   specularOn_, emissiveOn_, hasNormalMapping_, isNormalMapping_);
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.updateShadingText_ > " + string(kException.what()));
	}
}



void Scene::updateCameraText_() const {
	try {
		unsigned int id = 0u;
		unsigned int nCameras = static_cast<unsigned int>(cameras_.size());
		vec3 position = vec3(0.0f); vec3 lookAt = vec3(0.0f);
		float fieldOfView = 0.0f;

		if (pActiveCamera_) {
			id = pActiveCamera_->getId();
			position = *(pActiveCamera_->getPosition());
			lookAt = *(pActiveCamera_->getLookAt());

			switch (pActiveCamera_->getType()) {
			case BaseCamera::CameraType::PERSPECTIVE:
				fieldOfView = static_cast<PerspectiveCamera*>(pActiveCamera_)->getFieldOfView();
			case BaseCamera::CameraType::ORTHOGRAPHIC:
				break;
			}
		}

		pInfo_->setCameraText(id, nCameras, BaseCamera::MAX_NUMBER_OF_CAMERAS, position, lookAt, fieldOfView);
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.updateCameraText_ > " + string(kException.what()));
	}
}



void Scene::renderToScreen_(double currentTime, unsigned int fps) {
	const unsigned int kColorTexture = 1u;

	glViewport(0, 0, static_cast<GLsizei>(windowSize_.x), static_cast<GLsizei>(windowSize_.y));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	try {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		renderScene_();

		glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
		renderInfo_(currentTime, fps);

	}
	catch (const exception& kException) {
		throw runtime_error("Scene.renderToScreen_ > " + string(kException.what()));
	}
}



void Scene::renderScene_() {
	MainProgram::ProgramMode programMode;

	switch (shadingModel_) {
	case Scene::ShadingModel::NO_SHADING:
		programMode = MainProgram::ProgramMode::NO_SHADING;
		break;
	case Scene::ShadingModel::FLAT:
		programMode = MainProgram::ProgramMode::FLAT;
		break;
	case Scene::ShadingModel::GOURAUD:
		programMode = MainProgram::ProgramMode::GOURAUD;
		break;
	case Scene::ShadingModel::PHONG:
		programMode = isNormalMapping_ ? MainProgram::ProgramMode::NORMAL_MAPPING : MainProgram::ProgramMode::PHONG;
		break;
	default:
		return;
	}

	try {
		for (const Mesh& ikMesh : meshes_)
			renderMesh_(&ikMesh, programMode);		
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.renderScene_ > " + string(kException.what()));
	}
}



void Scene::renderInfo_(double currentTime, unsigned int fps) const {
	double time = 0.0;
	bool hasRotation = (rotationSpeed_ != 0.0f);

	if (hasRotation) {
		time = lastStopTime_ - lastResetTime_;
		if (isRotating_) time += currentTime - lastStartTime_;
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	try {
		pInfo_->displayMainInfo();
		pInfo_->displayTime(static_cast<float>(time), hasRotation);
		pInfo_->displayFps(fps);

		pInfo_->displaySceneInfo();
		pInfo_->displayShadingInfo();
		pInfo_->displayCameraInfo();
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.renderInfo_ > " + string(kException.what()));
	}
}



void Scene::renderMesh_(const Face* pkMesh, MainProgram::ProgramMode programMode) const {
	try {
		pMainShaderManager_->startProgram(programMode);

		mat4 modelMatrix = cursorRotationMatrix_ * rotationMatrix_ * (*(pkMesh->getModelMatrix()));
		mat4 modelViewMatrix = (*(pActiveCamera_->getViewMatrix())) * modelMatrix;
		mat4 modelViewProjMatrix = (*(pActiveCamera_->getViewProjectionMatrix())) * modelMatrix;
		mat4 normalMatrix = glm::transpose(glm::inverse(modelViewMatrix));

		pMainShaderManager_->setModelViewMatrix(modelViewMatrix);
		pMainShaderManager_->setModelViewProjectionMatrix(modelViewProjMatrix);
		pMainShaderManager_->setNormalMatrix(normalMatrix);

		if ((isWireframe_ && !isSolid_) || pkMesh->isWireframe())
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		
		const Material* pkMaterial = pkMesh->getMaterial();
		if (pkMaterial) 
			renderMaterial_(pkMesh, pkMaterial, modelMatrix, false);

		pMainShaderManager_->stopProgram();
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.renderMesh_ > " + string(kException.what()));
	}
}



void Scene::renderMaterial_(const Face* pkMesh, const Material* pkMaterial, const mat4& kModelMatrix, bool twoSided) const {
	try {
		pMainShaderManager_->setMaterialParameters(pkMaterial->getShadingModel());
		pMainShaderManager_->setMaterialParameters(*(pkMaterial->getAmbientColor()), *(pkMaterial->getDiffuseColor()),
			                                       *(pkMaterial->getSpecularColor()), *(pkMaterial->getEmissiveColor()));
		pMainShaderManager_->setMaterialParameters(pkMaterial->getShininess(), pkMaterial->getOpacity());
		
		const ColorTexture* pkDiffuseTexture = pkMaterial->getDiffuseTexture();
		const ColorTexture* pkSpecularTexture = pkMaterial->getSpecularTexture();
		const ColorTexture* pkEmissiveTexture = pkMaterial->getEmissiveTexture();
		const ColorTexture* pkNormalMapTexture = pkMaterial->getNormalMapTexture();

		if (pkDiffuseTexture) {
			pkDiffuseTexture->startReading(MainShaderManager::getDiffuseTextureUnit());
			pMainShaderManager_->setDiffuseTexParameters(true, pkDiffuseTexture->isDDS());
		}
		else pMainShaderManager_->setDiffuseTexParameters(false, false);

		if (pkSpecularTexture) {
			pkSpecularTexture->startReading(MainShaderManager::getSpecularTextureUnit());
			pMainShaderManager_->setSpecularTexParameters(true, pkSpecularTexture->isDDS());
		}
		else pMainShaderManager_->setSpecularTexParameters(false, false);

		if (pkEmissiveTexture) {
			pkEmissiveTexture->startReading(MainShaderManager::getEmissiveTextureUnit());
			pMainShaderManager_->setEmissiveTexParameters(true, pkEmissiveTexture->isDDS());
		}
		else pMainShaderManager_->setEmissiveTexParameters(false, false);

		if (pkNormalMapTexture) {
			pkNormalMapTexture->startReading(MainShaderManager::getNormalMapTextureUnit());
			pMainShaderManager_->setNormalTexParameters(true, pkNormalMapTexture->isDDS());
		}
		else pMainShaderManager_->setNormalTexParameters(false, false);

		pMainShaderManager_->render(pkMesh);

		if (pkDiffuseTexture) pkDiffuseTexture->stopReading();
		if (pkSpecularTexture) pkSpecularTexture->stopReading();
		if (pkEmissiveTexture) pkEmissiveTexture->stopReading();
		if (pkNormalMapTexture) pkNormalMapTexture->stopReading();
	}
	catch (const exception& kException) {
		throw runtime_error("Scene.renderMaterial_ > " + string(kException.what()));
	}
}



BaseCamera* Scene::getCamera_(unsigned int id) const {
	for (BaseCamera* iCamera : cameras_)
		if (iCamera->getId() == id)
			return iCamera;
	return nullptr;
}



BaseLight* Scene::getLight_(unsigned int id) const {
	for (BaseLight* iLight : lights_)
		if (iLight->getId() == id)
			return iLight;
	return nullptr;
}



Mesh* Scene::getMesh_(unsigned int modelId, unsigned int meshId) const {
	for (const Mesh& ikMesh : meshes_)
		if (ikMesh.getModelId() == modelId && ikMesh.getId() == meshId)
			return const_cast<Mesh*> (&ikMesh);
	return nullptr;
}
