/**
 * Author: Oldrin Barbulescu
 * Last modified: Feb 3, 2025
 **/

#ifndef MAIN_PROGRAM_H
#define MAIN_PROGRAM_H

#include <GL/gl3w.h>

#include "BaseProgram.h"
#include "light/light/BaseLight.h"
#include "mesh/mesh/Face.h"

#include <exception>
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>

using std::cout;
using std::endl;
using std::exception;
using std::list;
using std::runtime_error;
using std::string;
using std::to_string;



class MainProgram : public BaseProgram {
public:
	enum class ProgramMode { NO_SHADING = 1u, FLAT = 2u, GOURAUD = 3u, PHONG = 4u, NORMAL_MAPPING = 5u };


	MainProgram();
	virtual ~MainProgram();


	// init: link, setAttribPointers (for each object 'pMesh')
	//############################################################################
	void link(MainProgram::ProgramMode programMode, list<GLuint>& rVertexShaderList, list<GLuint>& rFragmentShaderList);

	void setAttribPointers(Face* pMesh) const;	

		
	// render (object 'pkMesh'): 1) start
	//                           2) [set...]
	//                           3) [render]
	//                           4) stop
	//############################################################################
	virtual void start() const;
	//-> void stop() const;

	void render(const Face* pkMesh) const;

	void setAmbientLightOn(GLuint on) const;
	void setDiffuseLightOn(GLuint on) const;
	void setSpecularLightOn(GLuint on) const;
	void setEmissiveLightOn(GLuint on) const;

	void setLightEyeDirection(const GLfloat* pkDirection) const;
	void setLightColor(const GLfloat* pkAmbient, const GLfloat* pkDiffuse, const GLfloat* pkSpecular) const;

	void setModelViewMatrix(const GLfloat* pkMatrix) const;
	void setModelViewProjectionMatrix(const GLfloat* pkMatrix) const;
	void setNormalMatrix(const GLfloat* pkMatrix) const;

	void setMaterialShadingModel(GLuint shadingModel) const;
	void setMaterialColor(const GLfloat* pkAmbient, const GLfloat* pkDiffuse,
		                  const GLfloat* pkSpecular, const GLfloat* pkEmissive) const;
	void setMaterialShininess(GLfloat shininess) const;
	void setMaterialOpacity(GLfloat opacity) const;

	void setDiffuseTexParameters(GLuint hasTexture, GLuint compressed) const;
	void setSpecularTexParameters(GLuint hasTexture, GLuint compressed) const;
	void setEmissiveTexParameters(GLuint hasTexture, GLuint compressed) const;
	void setNormalTexParameters(GLuint hasTexture, GLuint compressed) const;

	void setDiffuseTextureUnit(GLint textureUnit) const;
	void setSpecularTextureUnit(GLint textureUnit) const;
	void setEmissiveTextureUnit(GLint textureUnit) const;
	void setNormalMapTextureUnit(GLint textureUnit) const;


	// get
	//############################################################################
	//-> bool isLinked() const;
	//-> bool isInstalled() const;

private:
	MainProgram(const MainProgram&);
	const MainProgram& operator=(const MainProgram&) {}

	void bindAttribLocations_(MainProgram::ProgramMode programMode);
	void bindOutputLocations_(MainProgram::ProgramMode programMode);
	void queryUniformLocations_(MainProgram::ProgramMode programMode);

	struct SHD_LIGHT {
		GLint eDirection = -1;
		GLint ambientColor = -1, diffuseColor = -1, specularColor = -1;
	} shdLight_;

	struct SHD_MATERIAL {
		GLint shadingModel = -1;
		GLint ambientColor = -1, diffuseColor = -1, specularColor = -1, emissiveColor = -1;
		GLint shininess = -1;
		GLint opacity = -1;
	} shdMaterial_;

	struct SHD_SCENE {
		GLint ambientLightOn = -1, diffuseLightOn = -1, specularLightOn = -1, emissiveLightOn = -1;	
	} shdScene_;

	GLint shdModelViewMatrix_, shdModelViewProjectionMatrix_, shdNormalMatrix_;

	GLint shdHasDiffuseTexture_, shdHasSpecularTexture_, shdHasEmissiveTexture_, shdHasNormalMapTexture_;
	GLint shdDiffuseTexSampler_, shdSpecularTexSampler_, shdEmissiveTexSampler_, shdNormalMapTexSampler_;
	GLint shdDiffuseCompressed_, shdSpecularCompressed_, shdEmissiveCompressed_, shdNormalMapCompressed_;
};

#endif
