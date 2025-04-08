/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 13, 2021
 **/

#ifndef SHADER_H
#define SHADER_H

#include <GL/gl3w.h>

#include <cstdio>
#include <exception>
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>

using std::cout;
using std::endl;
using std::exception;
using std::FILE;
using std::list;
using std::runtime_error;
using std::size_t;
using std::string;
using std::to_string;



class Shader{
public:
	Shader(GLenum shaderType);
	~Shader();


	// init: 1) readSourceCode
	//       2) compile
	//############################################################################
	void readSourceCode(const string& kPath, const list<string>& kFileList, const string& kHeader);
	void compile();


	// get
	//############################################################################
	GLenum getShaderType() const;

	string getPath() const;
	list<string> getFileList() const;
	string getHeader() const;

	bool isCompiled() const;
	
	GLuint getId() const;
	string toString() const;

private:
	Shader(const Shader&);
	const Shader& operator=(const Shader&) {}

	void readFile_(const char* pkFileName, unsigned int fileNo);
	string getType_() const;
	string toString_() const;

	GLuint id_;
	GLenum shaderType_;
	string path_, header_, toStr_;
	list<string> fileList_;	
	GLchar** ppSource_;
	GLsizei nFiles_;
	bool compiled_;
};

#endif
