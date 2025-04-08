/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 31, 2025
 **/

#ifndef BASE_SHADER_MANAGER_H
#define BASE_SHADER_MANAGER_H

#include <GL/gl3w.h>

#include "shader/shader/Shader.h"
#include "shader/shaderProgram/BaseProgram.h"

#include <glm/vec2.hpp>

#include <exception>
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>
#include <utility>

using glm::uvec2;

using std::cout;
using std::endl;
using std::exception;
using std::list;
using std::pair;
using std::runtime_error;
using std::string;
using std::to_string;



class BaseShaderManager {
public:
	virtual ~BaseShaderManager();

	void compileShaders();

	virtual void setWindowSize(const uvec2& kSize);

	void stopProgram() const;	

protected:
	BaseShaderManager(const uvec2& kWindowSize);

	void addVertexShaderSourceCode_(BaseProgram::ProgramMode programMode,
		                            const string& kPath, const list<string>& kFileList, const string& kHeader);
	void addFragmentShaderSourceCode_(BaseProgram::ProgramMode programMode, 
		                              const string& kPath, const list<string>& kFileList, const string& kHeader);

	void linkProgram_(BaseProgram::ProgramMode programMode, list<GLuint>& rVertexShaderList, list<GLuint>& rFragmentShaderList);
	
	BaseProgram** ppPrograms_;
	BaseProgram* pCurrentProgram_;
	unsigned int nPrograms_;

	uvec2 windowSize_;

private:
	BaseShaderManager(const BaseShaderManager&);
	const BaseShaderManager& operator=(const BaseShaderManager&) {}

	list<Shader> vertexShaders_, fragmentShaders_;
	list<pair<BaseProgram::ProgramMode, Shader*>> vertexShaderRef_, fragmentShaderRef_;
};

#endif
