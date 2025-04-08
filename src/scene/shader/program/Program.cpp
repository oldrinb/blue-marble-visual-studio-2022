/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 14, 2021
 **/

#include "Program.h"



GLuint Program::installedProgramId_ = 0u;



Program::Program(): id_(0u), maxDrawBuffers_(0), maxVertexAttribs_(0),
                    vsIds_(), gsIds_(), fsIds_(), toStr_(), linked_(false) {
	id_ = glCreateProgram();
	if (id_ == 0u) throw runtime_error("Program|Create program failed.");

	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuffers_);
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs_);

	//cout << "Program " << id_ << " created." << endl;
}



Program::~Program() {
	glDeleteProgram(id_);

	//cout << "Program " << id_ << " deleted." << endl;
}



void Program::setAttribLocation(const GLchar* pkName, GLuint location) const {
	if (location < static_cast<GLuint>(maxVertexAttribs_))
		glBindAttribLocation(id_, location, pkName);
	else throw runtime_error("Program.setAttribLocation|Attribute location exceeds maximum number allowed.");
}



void Program::setOutputLocation(const GLchar* pkName, GLuint location) const {
	if (location < static_cast<GLuint>(maxDrawBuffers_))
		glBindFragDataLocation(id_, location, pkName);
	else throw runtime_error("Program.setOutputLocation|Output location exceeds maximum number allowed.");
}



void Program::link(list<GLuint>& rVertexShaderList, list<GLuint>& rFragmentShaderList) {
	list<GLuint> geometryShaderList;
	link(rVertexShaderList, geometryShaderList, rFragmentShaderList);
}



void Program::link(list<GLuint>& rVertexShaderList, list<GLuint>& rGeometryShaderList, list<GLuint>& rFragmentShaderList) {
	if (rVertexShaderList.empty())
		throw runtime_error("Program.link|No vertex shader provided.");
	if (rFragmentShaderList.empty())
		throw runtime_error("Program.link|No fragment shader provided.");
	rVertexShaderList.sort(); rVertexShaderList.unique();
	rFragmentShaderList.sort(); rFragmentShaderList.unique();

	if (*rVertexShaderList.begin() == 0u)
		throw runtime_error("Program.link|Invalid vertex shader id value.");
	if (*rFragmentShaderList.begin() == 0u)
		throw runtime_error("Program.link|Invalid fragment shader id value.");

	if (rGeometryShaderList.empty()) {
		if (linked_ && rVertexShaderList == vsIds_ && rFragmentShaderList == fsIds_)
			throw runtime_error("Program.link|Program " + to_string(id_) + " already linked.");
	}
	else {
		rGeometryShaderList.sort(); rGeometryShaderList.unique();
		if (*rGeometryShaderList.begin() == 0u)
			throw runtime_error("Program.link|Invalid geometry shader id value.");
		if (linked_ && rVertexShaderList == vsIds_ && rGeometryShaderList == gsIds_ && rFragmentShaderList == fsIds_)
			throw runtime_error("Program.link|Program " + to_string(id_) + " already linked.");
	}
		
	for (GLuint iShader: rVertexShaderList)
		glAttachShader(id_, iShader);
	for (GLuint iShader : rGeometryShaderList)
		glAttachShader(id_, iShader);
	for (GLuint iShader : rFragmentShaderList)
		glAttachShader(id_, iShader);
	 
	glLinkProgram(id_);

	for (GLuint iShader : rVertexShaderList)
		glDetachShader(id_, iShader);
	for (GLuint iShader : rGeometryShaderList)
		glDetachShader(id_, iShader);
	for (GLuint iShader : rFragmentShaderList)
		glDetachShader(id_, iShader);

	GLint linkedStatus;
	glGetProgramiv(id_, GL_LINK_STATUS, &linkedStatus);
	if (linkedStatus == GL_FALSE) {
		string errorMessage = "Program.link|Program linking failed. ";
		GLint len;
		glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &len);
		if (len > 0) {
			GLchar* pLog = new GLchar[static_cast<size_t>(len)];
			glGetProgramInfoLog(id_, len, &len, pLog);
			errorMessage += pLog;
			delete[] pLog;
		}
		throw runtime_error(errorMessage);
	}

	linked_ = true;
	vsIds_ = rVertexShaderList;
	gsIds_ = rGeometryShaderList;
	fsIds_ = rFragmentShaderList;

	toStr_ = toString_();
	cout << "Program " << toStr_ << " succesfully linked." << endl;
}



void Program::start() const {
	if (linked_) {
		glUseProgram(id_);
		Program::installedProgramId_ = id_;
	}
	else throw runtime_error("Program.start|Program " + to_string(id_) + " not linked.");
}



void Program::stop() const {
	glUseProgram(0u);
	Program::installedProgramId_ = 0u;
}



void Program::setUniformi(GLint location, GLint value) const {
	if (linked_)
		if (isInstalled())
			glUniform1i(location, value);
		else throw runtime_error("Program.setUniformi|Program " + to_string(id_) + " not installed.");	
	else throw runtime_error("Program.setUniformi|Program " + to_string(id_) + " not linked.");
}



void Program::setUniformui(GLint location, GLuint value) const {
	if (linked_)
		if (isInstalled())
			glUniform1ui(location, value);
		else throw runtime_error("Program.setUniformui|Program " + to_string(id_) + " not installed.");
	else throw runtime_error("Program.setUniformui|Program " + to_string(id_) + " not linked.");	
}



void Program::setUniformf(GLint location, GLfloat value) const {
	if (linked_)
		if (isInstalled())
			glUniform1f(location, value);
		else throw runtime_error("Program.setUniformf|Program " + to_string(id_) + " not installed.");
	else throw runtime_error("Program.setUniformf|Program " + to_string(id_) + " not linked.");	
}



void Program::setUniformVector2i(GLint location, const GLint* pkVector) const {
	if (linked_)
		if (isInstalled())
			glUniform2iv(location, 1, pkVector);
		else throw runtime_error("Program.setUniformVector2i|Program " + to_string(id_) + " not installed.");
	else throw runtime_error("Program.setUniformVector2i|Program " + to_string(id_) + " not linked.");	
}



void Program::setUniformVector2ui(GLint location, const GLuint* pkVector) const {
	if (linked_)
		if (isInstalled())
			glUniform2uiv(location, 1, pkVector);
		else throw runtime_error("Program.setUniformVector2ui|Program " + to_string(id_) + " not installed.");
	else throw runtime_error("Program.setUniformVector2ui|Program " + to_string(id_) + " not linked.");	
}



void Program::setUniformVector2f(GLint location, const GLfloat* pkVector) const {
	if (linked_)
		if (isInstalled())
			glUniform2fv(location, 1, pkVector);
		else throw runtime_error("Program.setUniformVector2f|Program " + to_string(id_) + " not installed.");
	else throw runtime_error("Program.setUniformVector2f|Program " + to_string(id_) + " not linked.");	
}



void Program::setUniformVector3f(GLint location, const GLfloat* pkVector) const {
	if (linked_)
		if (isInstalled())
			glUniform3fv(location, 1, pkVector);
		else throw runtime_error("Program.setUniformVector3f|Program " + to_string(id_) + " not installed.");
	else throw runtime_error("Program.setUniformVector3f|Program " + to_string(id_) + " not linked.");	
}



void Program::setUniformVector4f(GLint location, const GLfloat* pkVector) const {
	if (linked_)
		if (isInstalled())
			glUniform4fv(location, 1, pkVector);
		else throw runtime_error("Program.setUniformVector4f|Program " + to_string(id_) + " not installed.");
	else throw runtime_error("Program.setUniformVector4f|Program " + to_string(id_) + " not linked.");	
}



void Program::setUniformMatrix4f(GLint location, const GLfloat* pkMatrix) const {
	if (linked_)
		if (isInstalled())
			glUniformMatrix4fv(location, 1, GL_FALSE, pkMatrix);
		else throw runtime_error("Program.setUniformMatrix4f|Program " + to_string(id_) + " not installed.");
	else throw runtime_error("Program.setUniformMatrix4f|Program " + to_string(id_) + " not linked.");	
}



GLint Program::getAttribLocation(const GLchar* pkName) const {
	if (linked_) {
		GLint location = glGetAttribLocation(id_, pkName);
		if (location == -1)
			throw runtime_error(string("Program.getAttribLocation|Attribute location not found: ") + pkName);
		else return location;
	}
	else throw runtime_error("Program.getAttribLocation|Program " + to_string(id_) + " not linked.");
}



GLint Program::getUniformLocation(const GLchar* pkName) const {
	if (linked_) {
		GLint location = glGetUniformLocation(id_, pkName);
		if (location == -1)
			throw runtime_error(string("Program.getUniformLocation|Uniform location not found: ") + pkName);
		else return location;
	}
	else throw runtime_error("Program.getUniformLocation|Program " + to_string(id_) + " not linked.");
}



bool Program::isLinked() const {
	return linked_;
}



bool Program::isInstalled() const {
	return Program::installedProgramId_ == id_;
}



GLuint Program::getId () const {
	return id_;
}



string Program::toString() const {
	if (linked_) return toStr_;
	else throw runtime_error("Program.toString|Program " + to_string(id_) + " not linked.");
}



string Program::toString_() const {
	string result = to_string(id_) + " (vs: ";

	for (GLuint iShader : vsIds_)
		result += to_string(iShader) + " ";
	result.erase(result.end() - 1u);

	if (!gsIds_.empty()) {
		result += ", gs: ";
		for (GLuint iShader : gsIds_)
			result += to_string(iShader) + " ";
		result.erase(result.end() - 1u);
	}	
	
	result += ", fs: ";
	for (GLuint iShader : fsIds_)
		result += to_string(iShader) + " ";
	result.erase(result.end() - 1u);
	result += ")";

	return result;
}
