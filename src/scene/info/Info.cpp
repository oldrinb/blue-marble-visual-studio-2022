/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 30, 2025
 **/

#include "Info.h"



vec3 Info::getTextColor_() {
	return vec3(72.0f / 255.0f, 240.0f / 255.0f, 255.0f / 255.0f);
}



vec3 Info::getActiveTextColor_() {
	return vec3(1.0f, 1.0f, 0.0f);
}



vec3 Info::getInactiveTextColor_() {
	return vec3(0.4f, 0.4f, 0.4f);
}



uvec2 Info::getWindowMinSize_() {
	return uvec2(750u, 400u);
}



uvec2 Info::getWindowMinSizeSmall_() {
	return uvec2(250u, 150u);
}



Info::Info(const uvec2& kWindowSize): pkTexture_(nullptr), pText2dShaderManager_(nullptr),	
	                                  pTimeText_(nullptr), pFpsText_(nullptr), pMainText_(nullptr), pMainInverseText_(nullptr),
                                      pSceneText_(nullptr), pSceneInverseText_(nullptr), pShadingText_(nullptr),
	                                  pShadingInverseText_(nullptr), pCameraText_(nullptr), pCameraInverseText_(nullptr),
	                                  windowSize_(kWindowSize), enabled_(false), on_(true) {
	if (kWindowSize.x == 0u || kWindowSize.y == 0u)
		throw runtime_error("Info|Invalid window size value.");

	enabled_ = (kWindowSize.x > Info::getWindowMinSize_().x && kWindowSize.y > Info::getWindowMinSize_().y);

	pTimeText_ = new Text2D(kWindowSize);
	pFpsText_ = new Text2D(kWindowSize);
	pMainText_ = new Text2D(kWindowSize);
	pMainInverseText_ = new Text2D(kWindowSize);
	pSceneText_ = new Text2D(kWindowSize);
	pSceneInverseText_ = new Text2D(kWindowSize);
	pShadingText_ = new Text2D(kWindowSize);
	pShadingInverseText_ = new Text2D(kWindowSize);
	pCameraText_ = new Text2D(kWindowSize);
	pCameraInverseText_ = new Text2D(kWindowSize);

	try {
		pText2dShaderManager_ = new Text2dShaderManager(kWindowSize);
	}
	catch (const exception& kException) {
		delete pTimeText_;
		delete pFpsText_;
		delete pMainText_;
		delete pMainInverseText_;
		delete pSceneText_;
		delete pSceneInverseText_;
		delete pShadingText_;
		delete pShadingInverseText_;
		delete pCameraText_;
		delete pCameraInverseText_;

		throw runtime_error("Info > " + string(kException.what()));
	}	

	//cout << "Scene info created." << endl;
}



Info::~Info() {
	if (pkTexture_) delete pkTexture_;

	delete pText2dShaderManager_;

	delete pTimeText_;
	delete pFpsText_;
	delete pMainText_;
	delete pMainInverseText_;
	delete pSceneText_;
	delete pSceneInverseText_;
	delete pShadingText_;
	delete pShadingInverseText_;
	delete pCameraText_;
	delete pCameraInverseText_;	

	//cout << "Scene info deleted." << endl;
}



void Info::addShaderSourceCode(Text2dProgram::ProgramMode programMode, const string& kPath,
	                           const list<string>& kVertexShaderFileList, const string& kVertexShaderHeader,
	                           const list<string>& kFragmentShaderFileList, const string& kFragmentShaderHeader) const {
	try {
		pText2dShaderManager_->addShaderSourceCode(programMode, kPath, 
			                   kVertexShaderFileList, kVertexShaderHeader, kFragmentShaderFileList, kFragmentShaderHeader);
	}
	catch (const exception& kException) {
		throw runtime_error("Info.addShaderSourceCode > " + string(kException.what()));
	}
}



void Info::addVertexShaderSourceCode(Text2dProgram::ProgramMode programMode,
	                                 const string& kPath, const list<string>& kFileList, const string& kHeader) const {
	try {
		pText2dShaderManager_->addVertexShaderSourceCode(programMode, kPath, kFileList, kHeader);
	}
	catch (const exception& kException) {
		throw runtime_error("Info.addVertexShaderSourceCode > " + string(kException.what()));
	}
}



void Info::addFragmentShaderSourceCode(Text2dProgram::ProgramMode programMode,
	                                   const string& kPath, const list<string>& kFileList, const string& kHeader) const {
	try {
		pText2dShaderManager_->addFragmentShaderSourceCode(programMode, kPath, kFileList, kHeader);
	}
	catch (const exception& kException) {
		throw runtime_error("Info.addFragmentShaderSourceCode > " + string(kException.what()));
	}
}



void Info::compileShaders() const {
	try {
		pText2dShaderManager_->compileShaders();
		pText2dShaderManager_->linkProgram(Text2dProgram::ProgramMode::TEXT_2D);
	}
	catch (const exception& kException) {
		throw runtime_error("Info.compileShaders > " + string(kException.what()));
	}
}



void Info::setText2DTexture(const string& kFilePath, const string& kFileName) {
	if (pkTexture_) throw runtime_error("Info.setText2DTexture|Texture text 2D already loaded.");

	try {
		pkTexture_ = new ColorTexture(kFilePath, kFileName, false, false, false);
	}
	catch (const exception& kException) {
		throw runtime_error("Info.setText2DTexture > " + string(kException.what()));
	}

	if (!pkTexture_->isDDS())
		throw runtime_error("Info.setText2DTexture|Wrong text 2D texture format.");
}



void Info::setMainText(bool hasRotation, bool isRotating) const {
	if (windowSize_.x >= Info::getWindowMinSizeSmall_().x && windowSize_.y >= Info::getWindowMinSizeSmall_().y) {
		try {
			vec4 offset = vec4(-0.1f, -0.1f, 0.1f, 0.15f);

			setSizeText_();
			setCloseText_(offset);
			setInfoText_(offset);
			setRotationText_(hasRotation, isRotating, offset);

			pMainText_->updateVertexBuffer();
			pMainInverseText_->updateVertexBuffer();
			pText2dShaderManager_->setAttribPointers(pMainText_);
			pText2dShaderManager_->setAttribPointers(pMainInverseText_);
		}
		catch (const exception& kException) {
			throw runtime_error("Info.setMainText > " + string(kException.what()));
		}
	}
}



void Info::setSceneText(bool hasSolid, bool isSolid, bool isWireframe) const {
	if (enabled_ && on_) {
		try {
			vec4 offset = vec4(-0.1f, -0.1f, 0.1f, 0.15f);

			setSceneTitle_();
			setRenderText_(hasSolid, isSolid, isWireframe, offset);
			
			pSceneText_->updateVertexBuffer();
			pSceneInverseText_->updateVertexBuffer();
			pText2dShaderManager_->setAttribPointers(pSceneText_);
			pText2dShaderManager_->setAttribPointers(pSceneInverseText_);
		}
		catch (const exception& kException) {
			throw runtime_error("Info.setSceneText > " + string(kException.what()));
		}
	}
}



void Info::setShadingText(bool flatShading, bool gouraudShading, bool phongShading,
	                      bool hasAmbient, bool hasDiffuse, bool hasSpecular, bool hasEmissive,
	                      bool ambientOn, bool diffuseOn, bool specularOn, bool emissiveOn,	                      
	                      bool hasNormalMapping, bool isNormalMapping) const {
	if (enabled_ && on_) {
		try {
			vec4 offset = vec4(-0.1f, -0.1f, 0.1f, 0.15f);
			
			setShadingTitle_();
			setShadingModelText_(flatShading, gouraudShading, phongShading, offset);
			setComponentsText_(hasAmbient, hasDiffuse, hasSpecular, hasEmissive, 
				               ambientOn, diffuseOn, specularOn, emissiveOn, offset);
			setNormalMappingText_(hasNormalMapping, isNormalMapping, offset);

			pShadingText_->updateVertexBuffer();
			pShadingInverseText_->updateVertexBuffer();
			pText2dShaderManager_->setAttribPointers(pShadingText_);
			pText2dShaderManager_->setAttribPointers(pShadingInverseText_);
		}
		catch (const exception& kException) {
			throw runtime_error("Info.setShadingText > " + string(kException.what()));
		}
	}
}



void Info::setCameraText(unsigned int activeCamera, unsigned int nCameras, unsigned int nMaxCameras,
	                     const vec3& kPosition, const vec3& kLookAt, float fieldOfView) const {
	if (enabled_ && on_) {
		try {
			vec4 offset = vec4(-0.1f, -0.1f, 0.1f, 0.15f);

			setCameraTitle_();
			setCamerasText_(activeCamera, nCameras, nMaxCameras, offset);
			setPositionText_(kPosition);
			setLookAtText_(kLookAt);
			setTranslateText_(offset);
			setRotateText_(offset);
			setFieldOfViewText_(fieldOfView, offset);

			pCameraText_->updateVertexBuffer();
			pCameraInverseText_->updateVertexBuffer();
			pText2dShaderManager_->setAttribPointers(pCameraText_);
			pText2dShaderManager_->setAttribPointers(pCameraInverseText_);
		}
		catch (const exception& kException) {
			throw runtime_error("Info.setCameraText > " + string(kException.what()));
		}
	}
}



void Info::setWindowSize(const uvec2& kSize) {
	if (kSize.x == 0u || kSize.y == 0u)
		throw runtime_error("Info.setWindowSize|Invalid window size value.");
	windowSize_ = kSize;

	enabled_ = (kSize.x > Info::getWindowMinSize_().x && kSize.y > Info::getWindowMinSize_().y);

	pTimeText_->setWindowSize(kSize);
	pFpsText_->setWindowSize(kSize);
	pMainText_->setWindowSize(kSize);
	pMainInverseText_->setWindowSize(kSize);
	pSceneText_->setWindowSize(kSize);
	pSceneInverseText_->setWindowSize(kSize);
	pShadingText_->setWindowSize(kSize);
	pShadingInverseText_->setWindowSize(kSize);
	pCameraText_->setWindowSize(kSize);
	pCameraInverseText_->setWindowSize(kSize);

	try {
		pText2dShaderManager_->setWindowSize(kSize);
	}
	catch (const exception& kException) {
		throw runtime_error("Info.setWindowSize > " + string(kException.what()));
	}
}



void Info::setOn() {
	on_ = true;
}



void Info::setOff() {
	on_ = false;
}



void Info::displayMainInfo() const {
	if (windowSize_.x >= Info::getWindowMinSizeSmall_().x && windowSize_.y >= Info::getWindowMinSizeSmall_().y) {
		try {
			bool inverse = true;
			renderText_(pMainText_, !inverse);
			renderText_(pMainInverseText_, inverse);
		}
		catch (const exception& kException) {
			throw runtime_error("Info.displayMainInfo > " + string(kException.what()));
		}
	}
}



void Info::displayTime(float time, bool hasRotation) const {
	if (windowSize_.x >= Info::getWindowMinSizeSmall_().x && windowSize_.y >= Info::getWindowMinSizeSmall_().y) {
		try {
			setTimeText_(time, hasRotation);

			pTimeText_->updateVertexBuffer();
			pText2dShaderManager_->setAttribPointers(pTimeText_);
			renderText_(pTimeText_, false);
		}
		catch (const exception& kException) {
			throw runtime_error("Info.displayTime > " + string(kException.what()));
		}		
	}	
}



void Info::displayFps(unsigned int fps) const {
	if (windowSize_.x >= Info::getWindowMinSizeSmall_().x && windowSize_.y >= Info::getWindowMinSizeSmall_().y) {
		try {
			setFpsText_(fps);

			pFpsText_->updateVertexBuffer();
			pText2dShaderManager_->setAttribPointers(pFpsText_);
			renderText_(pFpsText_, false);
		}
		catch (const exception& kException) {
			throw runtime_error("Info.displayFps > " + string(kException.what()));
		}		
	}
}



void Info::displaySceneInfo() const {
	if (enabled_ && on_) {
		try {
			bool inverse = true;
			renderText_(pSceneText_, !inverse);
			renderText_(pSceneInverseText_, inverse);
		}
		catch (const exception& kException) {
			throw runtime_error("Info.displaySceneInfo > " + string(kException.what()));
		}
	}
}



void Info::displayShadingInfo() const {
	if (enabled_ && on_) {
		try {
			bool inverse = true;
			renderText_(pShadingText_, !inverse);
			renderText_(pShadingInverseText_, inverse);
		}
		catch (const exception& kException) {
			throw runtime_error("Info.displayShadingInfo > " + string(kException.what()));
		}
	}		
}



void Info::displayCameraInfo() const {
	if (enabled_ && on_) {
		try {
			bool inverse = true;
			renderText_(pCameraText_, !inverse);
			renderText_(pCameraInverseText_, inverse);
		}
		catch (const exception& kException) {
			throw runtime_error("Info.displayCameraInfo > " + string(kException.what()));
		}
	}	
}



bool Info::isEnabled() const {
	return enabled_;
}



bool Info::isOn() const {
	return on_;
}



void Info::setSizeText_() const {
	try {
		pMainText_->addText("SIZE", vec2(1.0f, -32.0f), Info::getTextColor_());
		pMainText_->addText(to_string(windowSize_.x) + " x " + to_string(windowSize_.y), vec2(1.0f, -27.0f),
			                Info::getActiveTextColor_());
	}
	catch (const exception& kException) {
		throw runtime_error("Info.setSizeText_ > " + string(kException.what()));
	}
}



void Info::setCloseText_(const vec4& kOffset) const {
	try {
		pMainText_->addText("CLOSE", vec2(1.0f, -11.0f), Info::getTextColor_());
		pMainInverseText_->addText("ESC", vec2(1.0f, -5.0f), Info::getTextColor_(), kOffset);
	}
	catch (const exception& kException) {
		throw runtime_error("Info.setCloseText_ > " + string(kException.what()));
	}
}



void Info::setInfoText_(const vec4& kOffset) const {
	try {
		vec3 color = (enabled_) ? Info::getTextColor_() : Info::getInactiveTextColor_();
		if (enabled_ && on_)
			pMainText_->addText("HIDE INFO", vec2(2.2f, -32.0f), color);
		else pMainText_->addText("SHOW INFO", vec2(2.2f, -32.0f), color);
		pMainInverseText_->addText("I", vec2(2.2f, -22.0f), color, kOffset);
	}
	catch (const exception& kException) {
		throw runtime_error("Info.setInfoText_ > " + string(kException.what()));
	}
}



void Info::setRotationText_(bool hasRotation, bool isRotating, const vec4& kOffset) const {
	try {
		vec3 color = (hasRotation) ? Info::getTextColor_() : Info::getInactiveTextColor_();
		float offs = 0.0f;
		if (!hasRotation || !isRotating) {
			pMainText_->addText("START ROTATION", vec2(3.4f, -32.0f), color);
			offs = 1.0f;
		}
		else pMainText_->addText("STOP ROTATION", vec2(3.4f, -32.0f), color);
		pMainInverseText_->addText("SPACE", vec2(3.4f, -18.0f + offs), color, kOffset);

		pMainText_->addText("RESET", vec2(3.4f, -9.0f), color);
		pMainInverseText_->addText("R", vec2(3.4f, -3.0f), color, vec4(-0.15f, -0.05f, 0.1f, 0.15f));
	}
	catch (const exception& kException) {
		throw runtime_error("Info.setRotationText_ > " + string(kException.what()));
	}
}



void Info::setTimeText_(float time, bool hasRotation) const {
	try {
		vec3 color = (hasRotation) ? Info::getTextColor_() : Info::getInactiveTextColor_();
		unsigned int sec = static_cast<unsigned int>(std::floor(time));
		unsigned int dec = static_cast<unsigned int>(std::floor((time - static_cast<float>(sec)) * 100.0f));
		unsigned int min = sec / 60u;
		sec = sec % 60u;

		pTimeText_->addText("TIME: ", vec2(4.6f, -32.0f), color);
		if (min < 10u) pTimeText_->addText(to_string(min), vec2(4.6f, -25.0f), color);
		else pTimeText_->addText(to_string(min), vec2(4.6f, -26.0f), color);
		pTimeText_->addText(":", vec2(4.6f, -24.0f), color);
		if (sec < 10u) {
			pTimeText_->addText("0", vec2(4.6f, -23.0f), color);
			pTimeText_->addText(to_string(sec), vec2(4.6f, -22.0f), color);
		}
		else pTimeText_->addText(to_string(sec), vec2(4.6f, -23.0f), color);
		pTimeText_->addText(".", vec2(4.6f, -21.0f), color);
		if (dec < 10u) {
			pTimeText_->addText("0", vec2(4.6f, -20.0f), color);
			pTimeText_->addText(to_string(dec), vec2(4.6f, -19.0f), color);
		}
		else pTimeText_->addText(to_string(dec), vec2(4.6f, -20.0f), color);
	}
	catch (const exception& kException) {
		throw runtime_error("Info.setTimeText_ > " + string(kException.what()));
	}
}



void Info::setFpsText_(unsigned int fps) const {
	try {
		string token = to_string(fps);
		string zeros = string(3u - token.length(), '0');
		token = zeros + token;

		pFpsText_->addText("FPS: ", vec2(4.6f, -10.0f), Info::getTextColor_());
		pFpsText_->addText(token, vec2(4.6f, -5.0f), Info::getTextColor_());
	}
	catch (const exception& kException) {
		throw runtime_error("Info.setFpsText_ > " + string(kException.what()));
	}
}



void Info::setSceneTitle_() const {
	try {
		pSceneText_->addText("SCENE", vec2(1.0f, 2.0f), Info::getTextColor_(), true);
	}
	catch (const exception& kException) {
		throw runtime_error("Info.setSceneTitle_ > " + string(kException.what()));
	}
}



void Info::setRenderText_(bool hasSolid, bool isSolid, bool isWireframe, const vec4& kOffset) const {
	try {
		pSceneText_->addText("Render:", vec2(2.3f, 2.0f), Info::getTextColor_());

		vec3 color = Info::getInactiveTextColor_();
		if (hasSolid) {
			color = Info::getTextColor_();
			if (isSolid) color = Info::getActiveTextColor_();
		}
		pSceneText_->addText("SOLID", vec2(2.3f, 10.0f), color);

		if (isWireframe) color = Info::getActiveTextColor_();
		else color = Info::getTextColor_();
		pSceneText_->addText("WIREFRAME", vec2(2.3f, 16.0f), color);

		if (hasSolid) color = Info::getTextColor_();
		else color = Info::getInactiveTextColor_();
		pSceneInverseText_->addText("W", vec2(2.3f, 26.0f), color, kOffset);
	}
	catch (const exception& kException) {
		throw runtime_error("Info.setRenderText_ > " + string(kException.what()));
	}
}



void Info::setShadingTitle_() const {
	try {
		pShadingText_->addText("SHADING", vec2(3.9f, 2.0f), Info::getTextColor_(), true);
	}
	catch (const exception& kException) {
		throw runtime_error("Info.setShadingTitle_ > " + string(kException.what()));
	}
}



void Info::setShadingModelText_(bool flatShading, bool gouraudShading, bool phongShading, const vec4& kOffset) const {
	try {
		pShadingText_->addText("Shading model:", vec2(5.2f, 2.0f), Info::getTextColor_());

		vec3 color = (!flatShading && !gouraudShading && !phongShading) ? Info::getActiveTextColor_() : Info::getTextColor_();
		pShadingText_->addText("NO-SHADING", vec2(5.2f, 17.0f), color);
		color = (flatShading) ? Info::getActiveTextColor_() : Info::getTextColor_();
		pShadingText_->addText("FLAT", vec2(5.2f, 30.0f), color);
		color = (gouraudShading) ? Info::getActiveTextColor_() : Info::getTextColor_();
		pShadingText_->addText("GOURAUD", vec2(5.2f, 37.0f), color);
		color = (phongShading) ? Info::getActiveTextColor_() : Info::getTextColor_();
		pShadingText_->addText("PHONG", vec2(5.2f, 47.0f), color);

		pShadingInverseText_->addText("7", vec2(5.2f, 28.0f), Info::getTextColor_(), kOffset);
		pShadingInverseText_->addText("8", vec2(5.2f, 35.0f), Info::getTextColor_(), kOffset);
		pShadingInverseText_->addText("9", vec2(5.2f, 45.0f), Info::getTextColor_(), kOffset);
		pShadingInverseText_->addText("0", vec2(5.2f, 53.0f), Info::getTextColor_(), kOffset);
	}
	catch (const exception& kException) {
		throw runtime_error("Info.setShadingModelText_ > " + string(kException.what()));
	}
}



void Info::setComponentsText_(bool hasAmbient, bool hasDiffuse, bool hasSpecular, bool hasEmissive,
	                          bool ambientOn, bool diffuseOn, bool specularOn, bool emissiveOn, const vec4& kOffset) const {
	try {
		pShadingText_->addText("Components:", vec2(6.3f, 2.0f), Info::getTextColor_());

		vec3 color = Info::getInactiveTextColor_();
		if (hasAmbient) color = (ambientOn) ? Info::getActiveTextColor_() : Info::getTextColor_();
		pShadingText_->addText("Ambient", vec2(6.3f, 14.0f), color);
		color = (hasAmbient) ? Info::getTextColor_() : Info::getInactiveTextColor_();
		pShadingInverseText_->addText("A", vec2(6.3f, 22.0f), color, kOffset);

		color = Info::getInactiveTextColor_();
		if (hasSpecular) color = (specularOn) ? Info::getActiveTextColor_() : Info::getTextColor_();
		pShadingText_->addText("Specular", vec2(6.3f, 25.0f), color);
		color = (hasSpecular) ? Info::getTextColor_() : Info::getInactiveTextColor_();
		pShadingInverseText_->addText("S", vec2(6.3f, 34.0f), color, kOffset);

		color = Info::getInactiveTextColor_();
		if (hasDiffuse) color = (diffuseOn) ? Info::getActiveTextColor_() : Info::getTextColor_();
		pShadingText_->addText("Diffuse", vec2(6.3f, 37.0f), color);
		color = (hasDiffuse) ? Info::getTextColor_() : Info::getInactiveTextColor_();
		pShadingInverseText_->addText("D", vec2(6.3f, 45.0f), color, kOffset);

		color = Info::getInactiveTextColor_();
		if (hasEmissive) color = (emissiveOn) ? Info::getActiveTextColor_() : Info::getTextColor_();
		pShadingText_->addText("Emissive", vec2(6.3f, 48.0f), color);
		color = (hasEmissive) ? Info::getTextColor_() : Info::getInactiveTextColor_();
		pShadingInverseText_->addText("F", vec2(6.3f, 57.0f), color, kOffset);
	}
	catch (const exception& kException) {
		throw runtime_error("Info.setComponentsText_ > " + string(kException.what()));
	}
}



void Info::setNormalMappingText_(bool hasNormalMapping, bool isNormalMapping, const vec4& kOffset) const {
	try {
		pShadingText_->addText("Normal mapping:", vec2(7.4f, 2.0f), Info::getTextColor_());

		vec3 color = (hasNormalMapping) ? 
			         (isNormalMapping) ? Info::getActiveTextColor_() : Info::getTextColor_()
			                                                         : Info::getInactiveTextColor_();

		pShadingText_->addText("ON", vec2(7.4f, 18.0f), color);
		color = (hasNormalMapping && isNormalMapping) ? Info::getTextColor_() : Info::getActiveTextColor_();
		pShadingText_->addText("OFF", vec2(7.4f, 21.0f), color);
		color = (hasNormalMapping) ? Info::getTextColor_() : Info::getInactiveTextColor_();
		pShadingInverseText_->addText("N", vec2(7.4f, 25.0f), color, kOffset);
	}
	catch (const exception& kException) {
		throw runtime_error("Info.setNormalMappingText_ > " + string(kException.what()));
	}
}



void Info::setCameraTitle_() const {
	try {
		pCameraText_->addText("CAMERA", vec2(-7.4f, 2.0f), Info::getTextColor_(), true);
	}
	catch (const exception& kException) {
		throw runtime_error("Info.setCameraTitle_ > " + string(kException.what()));
	}
}



void Info::setCamerasText_(unsigned int activeCamera, unsigned int nCameras, unsigned int nMaxCameras, const vec4& kOffset) const {
	try {
		for (unsigned int i = 0u; i < nCameras; i++) {
			string text = "Camera " + to_string(i + 1u);
			float col = 2.0f + 17.0f * static_cast<float>(i);
			vec3 color = (activeCamera == i) ? Info::getActiveTextColor_() : Info::getTextColor_();
			pCameraText_->addText(text, vec2(-6.1f, col), color);
			pCameraInverseText_->addText("CTRL+" + to_string(i + 1u), vec2(-6.1f, col + 9.0f), Info::getTextColor_(), kOffset);
		}

		for (unsigned int i = nCameras; i < nMaxCameras; i++) {
			string text = "Camera " + to_string(i + 1u);
			float col = 2.0f + 17.0f * static_cast<float>(i);
			pCameraText_->addText(text, vec2(-6.1f, col), Info::getInactiveTextColor_());
			pCameraInverseText_->addText("CTRL+" + to_string(i + 1u), vec2(-6.1f, col + 9.0f), Info::getInactiveTextColor_(),
				                         kOffset);
		}
	}
	catch (const exception& kException) {
		throw runtime_error("Info.setCamerasText_ > " + string(kException.what()));
	}
}



void Info::setPositionText_(const vec3& kPosition) const {
	try {
		std::stringstream stream;
		pCameraText_->addText("Position =", vec2(-5.0f, 2.0f), Info::getTextColor_());
		stream << std::fixed << std::setprecision(2) << "(" << kPosition.x << ", " << kPosition.y << ", " << kPosition.z << ")";
		string text = stream.str();
		pCameraText_->addText(text, vec2(-5.0f, 13.0f), Info::getActiveTextColor_());
	}
	catch (const exception& kException) {
		throw runtime_error("Info.setPositionText_ > " + string(kException.what()));
	}
}



void Info::setLookAtText_(const vec3& kLookAt) const {
	try {
		std::stringstream stream;
		pCameraText_->addText("Look at =", vec2(-5.0f, 35.0f), Info::getTextColor_());
		stream << std::fixed << std::setprecision(2) << "(" << kLookAt.x << ", " << kLookAt.y << ", " << kLookAt.z << ")";
		string text = stream.str();
		pCameraText_->addText(text, vec2(-5.0f, 45.0f), Info::getActiveTextColor_());
	}
	catch (const exception& kException) {
		throw runtime_error("Info.setLookAtText_ > " + string(kException.what()));
	}
}



void Info::setTranslateText_(const vec4& kOffset) const {
	try {
		pCameraText_->addText("Translate x:", vec2(-3.9f, 2.0f), Info::getTextColor_());
		pCameraInverseText_->addText("LEFT", vec2(-3.9f, 15.0f), Info::getTextColor_(), kOffset);
		pCameraInverseText_->addText("RIGHT", vec2(-3.9f, 20.0f), Info::getTextColor_(), kOffset);
		pCameraText_->addText("y:", vec2(-3.9f, 27.0f), Info::getTextColor_());
		pCameraInverseText_->addText("UP", vec2(-3.9f, 30.0f), Info::getTextColor_(), kOffset);
		pCameraInverseText_->addText("DOWN", vec2(-3.9f, 33.0f), Info::getTextColor_(), kOffset);
		pCameraText_->addText("z:", vec2(-3.9f, 39.0f), Info::getTextColor_());
		pCameraInverseText_->addText("=", vec2(-3.9f, 42.0f), Info::getTextColor_(), kOffset);
		pCameraInverseText_->addText("-", vec2(-3.9f, 44.0f), Info::getTextColor_(), kOffset);
		pCameraText_->addText("/", vec2(-3.9f, 46.0f), Info::getTextColor_());
		pCameraInverseText_->addText("+", vec2(-3.9f, 48.0f), Info::getTextColor_(), kOffset);
		pCameraInverseText_->addText("_", vec2(-4.1f, 50.0f), Info::getTextColor_(), vec4(-0.1f, -0.1f, 0.3f, -0.05f));
		pCameraText_->addText("Reset:", vec2(-3.9f, 53.0f), Info::getTextColor_());
		pCameraInverseText_->addText("NUM 5", vec2(-3.9f, 60.0f), Info::getTextColor_(), kOffset);
	}
	catch (const exception& kException) {
		throw runtime_error("Info.setTranslateText_ > " + string(kException.what()));
	}
}



void Info::setRotateText_(const vec4& kOffset) const {
	try {
		pCameraText_->addText("Rotate x:", vec2(-2.8f, 2.0f), Info::getTextColor_());
		pCameraInverseText_->addText("NUM UP", vec2(-2.8f, 12.0f), Info::getTextColor_(), kOffset);
		pCameraInverseText_->addText("NUM DOWN", vec2(-2.8f, 19.0f), Info::getTextColor_(), kOffset);
		pCameraText_->addText("y:", vec2(-2.8f, 29.0f), Info::getTextColor_());
		pCameraInverseText_->addText("NUM LEFT", vec2(-2.8f, 32.0f), Info::getTextColor_(), kOffset);
		pCameraInverseText_->addText("NUM RIGHT", vec2(-2.8f, 41.0f), Info::getTextColor_(), kOffset);
		pCameraText_->addText("Reset:", vec2(-2.8f, 52.0f), Info::getTextColor_());
		pCameraInverseText_->addText("NUM 5", vec2(-2.8f, 59.0f), Info::getTextColor_(), kOffset);
	}
	catch (const exception& kException) {
		throw runtime_error("Info.setRotateText_ > " + string(kException.what()));
	}
}



void Info::setFieldOfViewText_(float fieldOfView, const vec4& kOffset) const {
	try {
		vec3 textColor = Info::getTextColor_();
		vec3 activeTextColor = Info::getActiveTextColor_();

		if (fieldOfView == 0.0f)
			textColor = activeTextColor = Info::getInactiveTextColor_();		

		std::stringstream stream;
		pCameraText_->addText("Field of view =", vec2(-1.7f, 2.0f), textColor);
		stream << std::setw(6) << std::fixed << std::setprecision(2) << fieldOfView;
		string text = stream.str();
		pCameraText_->addText(text, vec2(-1.7f, 18.0f), activeTextColor);

		pCameraText_->addText("deg.", vec2(-1.7f, 25.0f), textColor);
		pCameraText_->addText("Change:", vec2(-1.7f, 32.0f), textColor);
		pCameraInverseText_->addText("MOUSE SCROLL", vec2(-1.7f, 40.0f), textColor, kOffset);
		pCameraText_->addText("Reset:", vec2(-1.7f, 54.0f), textColor);
		pCameraInverseText_->addText("MOUSE MIDDLE", vec2(-1.7f, 61.0f), textColor, kOffset);
	}
	catch (const exception& kException) {
		throw runtime_error("Info.setFieldOfViewText_ > " + string(kException.what()));
	}
}



void Info::renderText_(const Text2D* pkText2D, bool inverse) const {
	if (pkTexture_) {
		try {
			pText2dShaderManager_->startProgram(Text2dProgram::ProgramMode::TEXT_2D);
			pText2dShaderManager_->setTextParameters(inverse);

			pkTexture_->startReading(Text2dShaderManager::getTextureUnit());
			pText2dShaderManager_->render(pkText2D);
			pkTexture_->stopReading();

			pText2dShaderManager_->stopProgram();
		}
		catch (const exception& kException) {
			throw runtime_error("Info.renderText_ > " + string(kException.what()));
		}		
	}
	else throw runtime_error("Info.renderText_|Texture text 2D not loaded yet.");
}
