/**
 * Author: Oldrin Barbulescu
 * Last modified: Feb 5, 2025
 **/

#ifndef SCENE_H
#define SCENE_H

#include <GL/gl3w.h>

#include "camera/BaseCamera.h"
#include "camera/PerspectiveCamera.h"
#include "info/Info.h"
#include "light/light/BaseLight.h"
#include "light/light/DirectionalLight.h"
#include "material/Material.h"
#include "mesh/mesh/Face.h"
#include "mesh/mesh/Mesh.h"
#include "mesh/triangle/TriangleList2D.h"
#include "model/Model3D.h"
#include "shader/shaderManager/MainShaderManager.h"
#include "shader/shaderProgram/MainProgram.h"
#include "shader/shaderProgram/Text2dProgram.h"
#include "texture/texture/ColorTexture.h"

#include <glm/geometric.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/matrix.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <glm/ext/matrix_transform.hpp>

#include <algorithm>
#include <cmath>
#include <exception>
#include <iostream>
#include <limits>
#include <list>
#include <stdexcept>
#include <string>
#include <vector>

using glm::mat3;
using glm::mat4;
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
using std::vector;



class Scene {
public:
	enum class ShadingModel { NO_SHADING = 1u, FLAT = 2u, GOURAUD = 3u, PHONG = 4u };

	
	Scene(const uvec2& kWindowSize);
	~Scene();

	
	// init: 1) add(...)ShaderSourceCode (for each 'programMode' in 'Main/Text2dProgram')
	//          import3DModel (for each model), setText2DTexture
	//       2) compileShaders, loadBufferData, addCamera, [setLight], [translate/scale/rotateMesh], [setMeshWireframe],
	//          [setMeshRotationSpeed], [setRotationSpeed]
	//       3) initializeSceneParameters
	//############################################################################
	void addShaderSourceCode(MainProgram::ProgramMode programMode, const string& kPath,
		                     const list<string>& kVertexShaderFileList, const string& kVertexShaderHeader,
		                     const list<string>& kFragmentShaderFileList, const string& kFragmentShaderHeader) const;
	void addVertexShaderSourceCode(MainProgram::ProgramMode programMode, const string& kPath,
		                           const list<string>& kFileList, const string& kHeader) const;
	void addFragmentShaderSourceCode(MainProgram::ProgramMode programMode, const string& kPath,
		                             const list<string>& kFileList, const string& kHeader) const;
	
	void addShaderSourceCode(Text2dProgram::ProgramMode programMode, const string& kPath,
		                     const list<string>& kVertexShaderFileList, const string& kVertexShaderHeader,
		                     const list<string>& kFragmentShaderFileList, const string& kFragmentShaderHeader) const;
	void addVertexShaderSourceCode(Text2dProgram::ProgramMode programMode, const string& kPath,
		                           const list<string>& kFileList, const string& kHeader) const;
	void addFragmentShaderSourceCode(Text2dProgram::ProgramMode programMode, const string& kPath,
		                             const list<string>& kFileList, const string& kHeader) const;
	
	void compileShaders() const;
	
	void setText2DTexture(const string& kFilePath, const string& kFileName) const;
	
	void import3DModel(const string& kFilePath, const string& kFileName, unsigned int postProcessSteps);
	void loadBufferData();
	
	// modelId = meshId = -1 -> all meshes
	void translateMesh(int modelId, int meshId, const vec3& kDistance);
	void scaleMesh(int modelId, int meshId, const vec3& kScale);
	void rotateMesh(int modelId, int meshId, float angle, const vec3& kAxis); // angle in degrees

	// modelId = meshId = -1 -> all meshes
	void setMeshWireframe(int modelId, int meshId, bool wireframe);
	void setMeshRotationSpeed(int modelId, int meshId, float value);// modelId = meshId = -1 -> all meshes
	
	void addCamera(const vec3& kPosition, const vec3& kLookAt, float fieldOfView); // fieldOfView in degrees

	void setLight(const vec3& kDirection, const vec3& kAmbientColor, const vec3& kDiffuseColor,
		                     const vec3& kSpecularColor);

	void setRotationSpeed(float value);
	
	void initializeSceneParameters();
	

	// set
	//############################################################################

	void setWindowSize(const uvec2& kSize);

	void toggleDisplayInfo() const;
	
	void toggleRotation(double currentTime);
	void resetRotation(double currentTime);
	
	void toggleWireframe();

	void setShadingModel(Scene::ShadingModel shadingModel);
	
	void toggleAmbientLight();
	void toggleDiffuseLight();
	void toggleSpecularLight();
	void toggleEmissiveLight();

	void toggleNormalMapping();

	void setActiveCamera(unsigned int id);
	
	void translateCameraRight(float distance) const;
	void translateCameraUp(float distance) const;
	void translateCameraForward(float distance) const;
	void translateCameraToCenter(float distance) const;

	void rotateCameraRight(float angle) const; // degrees
	void rotateCameraUp(float angle) const; // degrees

	void resetCamera() const;

	void setCameraFieldOfView(float angle) const; // degrees
	void changeCameraFieldOfView(float deltaAngle) const; // degrees
	void resetCameraFieldOfView() const;
	
	void updateMouseMovement(float angleX, float angleY); // degrees


	// render: 1) updateRotation
	//         2) render
	//############################################################################
	void updateRotation(float deltaTime);
	void render(double currentTime, unsigned int fps);

private:
	static const float kLengthEpsilon_;

	static float getMaxSceneRadius_();
	static float getMaxVerticalRotationAngle_();

	Scene(const Scene&);
	const Scene& operator=(const Scene&) {}

	void addPerspectiveCamera_(const vec3& kPosition, const vec3& kLookAt, float fieldOfView);	
	void setDirectionalLight_(const vec3& kDirection,
		           const vec3& kAmbientColor, const vec3& kDiffuseColor, const vec3& kSpecularColor);
	
	void initializeSceneParameters_();
	void initializeShadingParameters_();
	void initializeLightParameters_();
	void initializeCameraParameters_();
	
	void computeBoundingBox_();
	void computeCameraFrustrum_(BaseCamera* pCamera) const;
	
	void setShadingModel_(Scene::ShadingModel shadingModel);
	void setNormalMapping_(bool on);
	
	void toggleLightComponent_(BaseLight::LightComponent component);

	void updateRotationMatrix_();
	void updateCursorRotationMatrix_();

	void updateLightInEyeSpace_() const;

	void updateDisplayInfo_() const;

	void updateMainText_() const;
	void updateSceneText_() const;
	void updateShadingText_() const;
	void updateCameraText_() const;

	void renderToScreen_(double currentTime, unsigned int fps);		
	void renderScene_();

	void renderInfo_(double currentTime, unsigned int fps) const;	
	void renderMesh_(const Face* pkMesh, MainProgram::ProgramMode programMode) const;	
	void renderMaterial_(const Face* pkMesh, const Material* pkMaterial, const mat4& kModelMatrix, bool twoSided) const;
		
	BaseCamera* getCamera_(unsigned int id) const;
	BaseLight* getLight_(unsigned int id) const;
	Mesh* getMesh_(unsigned int modelId, unsigned int meshId) const;

	list<PerspectiveCamera> perspectiveCameras_;	
	list<BaseCamera*> cameras_;
	BaseCamera* pActiveCamera_;
	
	list<DirectionalLight> directionalLights_;
	list<BaseLight*> lights_;

	list<Material> materials_;
	list<Mesh> meshes_;
	list<ColorTexture> diffuseTextures_, specularTextures_, emissiveTextures_, normalMapTextures_;
	
	MainShaderManager* pMainShaderManager_;
	Info* pInfo_;
	
	mat4 rotationMatrix_, cursorRotationMatrix_;
	float rotationAngle_, rotationSpeed_;
	float cursorRotationAngleX_, cursorRotationAngleY_;
	bool isRotating_;
	double lastStartTime_, lastStopTime_, lastResetTime_;	

	Scene::ShadingModel shadingModel_;

	bool hasSolid_, hasWireframe_, isSolid_, isWireframe_;
	bool hasAmbientColor_, hasDiffuseColor_, hasSpecularColor_, hasEmissiveColor_;
	bool ambientOn_, diffuseOn_, specularOn_, emissiveOn_;
	bool hasNormalMapping_, isNormalMapping_;

	uvec2 windowSize_;
	float aspectRatio_;
};

#endif
