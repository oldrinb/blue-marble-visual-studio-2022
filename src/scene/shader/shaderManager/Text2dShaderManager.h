/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 14, 2021
 **/

#ifndef TEXT2D_SHADER_MANAGER_H
#define TEXT2D_SHADER_MANAGER_H

#include <GL/gl3w.h>

#include "BaseShaderManager.h"
#include "mesh/text/Text2D.h"
#include "shader/shaderProgram/Text2dProgram.h"

#include <glm/vec2.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <exception>
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>

using glm::uvec2;
using glm::value_ptr;

using std::cout;
using std::endl;
using std::exception;
using std::list;
using std::runtime_error;
using std::string;



class Text2dShaderManager : public BaseShaderManager {
public:
	static GLint getTextureUnit();


	Text2dShaderManager(const uvec2& kWindowSize);
	virtual ~Text2dShaderManager();


	// init: 1) addShaderSourceCode/addVertexShaderSourceCode/addFragmentShaderSourceCode (programMode = 'TEXT_2D')
	//       2) compileShaders
	//       3) linkProgram (programMode = 'TEXT_2D')
	//############################################################################
	void addShaderSourceCode(Text2dProgram::ProgramMode programMode, const string& kPath,
		                     const list<string>& kVertexShaderFileList, const string& kVertexShaderHeader,
		                     const list<string>& kFragmentShaderFileList, const string& kFragmentShaderHeader);
	void addVertexShaderSourceCode(Text2dProgram::ProgramMode programMode, const string& kPath,
		                           const list<string>& kFileList, const string& kHeader);
	void addFragmentShaderSourceCode(Text2dProgram::ProgramMode programMode, const string& kPath,
		                             const list<string>& kFileList, const string& kHeader);

	//-> void compileShaders();
	void linkProgram(Text2dProgram::ProgramMode programMode);


	// init: setAttribPointers (for each object 'pText2D')
	//############################################################################
	void setAttribPointers(Text2D* pText2D) const;


	// set
	//############################################################################
	virtual void setWindowSize(const uvec2& kSize);

	
	// render (object 'pkText2D'): 1) startProgram (programMode = 'TEXT_2D')
	//                             2) [setTextParameters]
	//                             3) [render]
	//                             4) stopProgram
	//############################################################################
	void startProgram(Text2dProgram::ProgramMode programMode);
	//-> void stopProgram() const;

	void render(const Text2D* pkText2D) const;	

	void setTextParameters(bool inverseColor) const;

private:
	Text2dShaderManager(const Text2dShaderManager&);
	const Text2dShaderManager& operator=(const Text2dShaderManager&) {}
	
	Text2dProgram* pText2dProgram_;
};

#endif
