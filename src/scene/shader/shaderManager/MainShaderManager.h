/**
 * Author: Oldrin Barbulescu
 * Last modified: Feb 3, 2025
 **/

#ifndef MAIN_SHADER_MANAGER_H
#define MAIN_SHADER_MANAGER_H

#include <GL/gl3w.h>

#include "BaseShaderManager.h"
#include "light/light/BaseLight.h"
#include "material/Material.h"
#include "mesh/mesh/Face.h"
#include "shader/shaderProgram/MainProgram.h"

#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <exception>
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>

using glm::mat4;
using glm::uvec2;
using glm::value_ptr;
using glm::vec3;

using std::cout;
using std::endl;
using std::exception;
using std::list;
using std::runtime_error;
using std::string;



class MainShaderManager : public BaseShaderManager {
public:
	static GLint getDiffuseTextureUnit();
	static GLint getSpecularTextureUnit();
	static GLint getEmissiveTextureUnit();
	static GLint getNormalMapTextureUnit();


	MainShaderManager(const uvec2& kWindowSize);
	virtual ~MainShaderManager();


	// init: 1) addShaderSourceCode/addVertexShaderSourceCode/addFragmentShaderSourceCode
	//          (for each programMode: 'NO_SHADING', 'FLAT', 'GOURAUD', 'PHONG', 'NORMAL_MAPPING')
	//       2) compileShaders
	//       3) linkProgram (for each programMode: 'NO_SHADING', 'FLAT', 'GOURAUD', 'PHONG', 'NORMAL_MAPPING')
	//############################################################################
	void addShaderSourceCode(MainProgram::ProgramMode programMode, const string& kPath,
		                     const list<string>& kVertexShaderFileList, const string& kVertexShaderHeader,
		                     const list<string>& kFragmentShaderFileList, const string& kFragmentShaderHeader);
	void addVertexShaderSourceCode(MainProgram::ProgramMode programMode, const string& kPath,
		                           const list<string>& kFileList, const string& kHeader);
	void addFragmentShaderSourceCode(MainProgram::ProgramMode programMode, const string& kPath,
		                             const list<string>& kFileList, const string& kHeader);

	//-> void compileShaders();
	void linkProgram(MainProgram::ProgramMode programMode);


	// init: setAttribPointers (for each object 'pMesh')
	//############################################################################
	void setAttribPointers(Face* pMesh) const;


	// set
	//############################################################################
	void setShadingParameters(bool ambientLightOn, bool diffuseLightOn, bool specularLightOn, bool emissiveLightOn) const;
	void setLightEyeDirection(const vec3& kDirection) const;
	void setLightParameters(const vec3& kAmbientColor, const vec3& kDiffuseColor, const vec3& kSpecularColor) const;

	
	// render (object 'pkMesh'): 
	//         1) startProgram (programMode = 'NO_SHADING' / 'FLAT' / 'GOURAUD' / 'PHONG' / 'NORMAL_MAPPING')
	//         2) [set...Matrix], [setMaterialParameters], [set...TexParameters]
	//         3) [render]
	//         4) stopProgram
	//############################################################################
	void startProgram(MainProgram::ProgramMode programMode);
	//-> void stopProgram() const;

	void render(const Face* pkMesh) const;	

	void setModelViewMatrix(const mat4& kMatrix) const;
	void setModelViewProjectionMatrix(const mat4& kMatrix) const;
	void setNormalMatrix(const mat4& kMatrix) const;

	void setMaterialParameters(Material::ShadingModel shadingModel) const;
	void setMaterialParameters(const vec3& kAmbientColor, const vec3& kDiffuseColor,
		                       const vec3& kSpecularColor, const vec3& kEmissiveColor) const;
	void setMaterialParameters(float shininess, float opacity) const;

	void setDiffuseTexParameters(bool hasTexture, bool compressed) const;
	void setSpecularTexParameters(bool hasTexture, bool compressed) const;
	void setEmissiveTexParameters(bool hasTexture, bool compressed) const;
	void setNormalTexParameters(bool hasTexture, bool compressed) const;

private:
	MainShaderManager(const MainShaderManager&);
	const MainShaderManager& operator=(const MainShaderManager&) {}
	
	MainProgram* pNoShadingProgram_, * pFlatShadingProgram_, * pGouraudShadingProgram_, 
		       * pPhongShadingProgram_, * pNormalMappingProgram_;
};

#endif
