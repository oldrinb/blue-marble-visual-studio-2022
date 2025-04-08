/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 30, 2025
 **/
 
#ifndef INFO_H
#define INFO_H

#include "mesh/text/Text2D.h"
#include "shader/shaderManager/Text2dShaderManager.h"
#include "shader/shaderProgram/Text2dProgram.h"
#include "texture/texture/ColorTexture.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cmath>
#include <exception>
#include <iomanip>
#include <iostream>
#include <list>
#include <sstream>
#include <stdexcept>
#include <string>

using glm::uvec2;
using glm::vec2;
using glm::vec3;
using glm::vec4;

using std::cout;
using std::endl;
using std::exception;
using std::list;
using std::runtime_error;
using std::string;
using std::to_string;



class Info {
public:
	Info(const uvec2& kWindowSize);
	~Info();


	// init: 1) addShaderSourceCode/addVertexShaderSourceCode/addFragmentShaderSourceCode (programMode = 'TEXT_2D')
	//       2) compileShaders
	//############################################################################
	void addShaderSourceCode(Text2dProgram::ProgramMode programMode, const string& kPath,
		                     const list<string>& kVertexShaderFileList, const string& kVertexShaderHeader,
		                     const list<string>& kFragmentShaderFileList, const string& kFragmentShaderHeader) const;
	void addVertexShaderSourceCode(Text2dProgram::ProgramMode programMode,
		                           const string& kPath, const list<string>& kFileList, const string& kHeader) const;
	void addFragmentShaderSourceCode(Text2dProgram::ProgramMode programMode,
		                             const string& kPath, const list<string>& kFileList, const string& kHeader) const;

	void compileShaders() const;


	// init
	//############################################################################
	void setText2DTexture(const string& kFilePath, const string& kFileName);


	// init
	//############################################################################
	void setMainText(bool hasRotation, bool isRotating) const;
	void setSceneText(bool hasSolid, bool isSolid, bool isWireframe) const;
	void setShadingText(bool flatShading, bool gouraudShading, bool phongShading,
		                bool hasAmbient, bool hasDiffuse, bool hasSpecular, bool hasEmissive,
		                bool ambientOn, bool diffuseOn, bool specularOn, bool emissiveOn,	                
		                bool hasNormalMapping, bool isNormalMapping) const;
	void setCameraText(unsigned int activeCamera, unsigned int nCameras, unsigned int nMaxCameras,
		               const vec3& kPosition, const vec3& kLookAt, float fieldOfView) const;
	
	
	// set
	//############################################################################
	void setWindowSize(const uvec2& kSize);

	void setOn();
	void setOff();


	// render
	//############################################################################
	void displayMainInfo() const;
	void displayTime(float time, bool hasRotation) const;
	void displayFps(unsigned int fps) const;
	
	void displaySceneInfo() const;
	void displayShadingInfo() const;
	void displayCameraInfo() const;


	// get
	//############################################################################
	bool isEnabled() const;
	bool isOn() const;

private:
	static vec3 getTextColor_();
	static vec3 getActiveTextColor_();
	static vec3 getInactiveTextColor_();

	static uvec2 getWindowMinSize_();
	static uvec2 getWindowMinSizeSmall_();

	Info(const Info&);
	const Info& operator=(const Info&) {}

	void setSizeText_() const;
	void setCloseText_(const vec4& kOffset) const;
	void setInfoText_(const vec4& kOffset) const;
	void setRotationText_(bool hasRotation, bool isRotating, const vec4& kOffset) const;
	void setTimeText_(float time, bool hasRotation) const;
	void setFpsText_(unsigned int fps) const;
	
	void setSceneTitle_() const;
	void setRenderText_(bool hasSolid, bool isSolid, bool isWireframe, const vec4& kOffset) const;

	void setShadingTitle_() const;
	void setShadingModelText_(bool flatShading, bool gouraudShading, bool phongShading, const vec4& kOffset) const;
	void setComponentsText_(bool hasAmbient, bool hasDiffuse, bool hasSpecular, bool hasEmissive,
		                    bool ambientOn, bool diffuseOn, bool specularOn, bool emissiveOn, const vec4& kOffset) const;
	void setNormalMappingText_(bool hasNormalMapping, bool isNormalMapping, const vec4& kOffset) const;
	
	void setCameraTitle_() const;
	void setCamerasText_(unsigned int activeCamera, unsigned int nCameras, unsigned int nMaxCameras, const vec4& kOffset) const;
	void setPositionText_(const vec3& kPosition) const;
	void setLookAtText_(const vec3& kLookAt) const;
	void setTranslateText_(const vec4& kOffset) const;
	void setRotateText_(const vec4& kOffset) const;
	void setFieldOfViewText_(float fieldOfView, const vec4& kOffset) const;

	void renderText_(const Text2D* pkText2D, bool inverse) const;

	const ColorTexture* pkTexture_;
	
	Text2dShaderManager* pText2dShaderManager_;

	Text2D* pTimeText_, * pFpsText_;
	Text2D* pMainText_, * pMainInverseText_;
	Text2D* pSceneText_, * pSceneInverseText_;
	Text2D* pShadingText_, * pShadingInverseText_;
	Text2D* pCameraText_, * pCameraInverseText_;
	
	uvec2 windowSize_;
	bool enabled_, on_;
};

#endif
