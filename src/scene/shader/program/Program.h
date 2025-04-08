/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 13, 2021
 **/

#ifndef PROGRAM_H
#define PROGRAM_H

#include <GL/gl3w.h>

#include <iostream>
#include <list>
#include <stdexcept>
#include <string>

using std::cout;
using std::endl;
using std::list;
using std::runtime_error;
using std::string;
using std::to_string;



class Program {
public:
	Program();
	~Program();


	// init: 1) setAttribLocation, [setOutputLocation]
	//       2) link
	//############################################################################
	void setAttribLocation(const GLchar* pkName, GLuint location) const;
	void setOutputLocation(const GLchar* pkName, GLuint location) const;

	void link(list<GLuint>& rVertexShaderList, list<GLuint>& rFragmentShaderList);
	void link(list<GLuint>& rVertexShaderList, list<GLuint>& rGeometryShaderList, list<GLuint>& rFragmentShaderList);


	// render: 1) start
	//         2) [setUniform...]
	//         3) stop
	//############################################################################
	void start() const;
	void stop() const;

	void setUniformi(GLint location, GLint value) const;
	void setUniformui(GLint location, GLuint value) const;
	void setUniformf(GLint location, GLfloat value) const;

	void setUniformVector2i(GLint location, const GLint* pkVector) const;
	void setUniformVector2ui(GLint location, const GLuint* pkVector) const;
	void setUniformVector2f(GLint location, const GLfloat* pkVector) const;

	void setUniformVector3f(GLint location, const GLfloat* pkVector) const;
	void setUniformVector4f(GLint location, const GLfloat* pkVector) const;

	void setUniformMatrix4f(GLint location, const GLfloat* pkMatrix) const;
		

	// get
	//############################################################################
	GLint getAttribLocation(const GLchar* pkName) const;
	GLint getUniformLocation(const GLchar* pkName) const;

	bool isLinked() const;
	bool isInstalled() const;

	GLuint getId() const;
	string toString() const;	
	
private:
	static GLuint installedProgramId_;

	Program(const Program&);
	const Program& operator=(const Program&) {}

	string toString_() const;

	GLuint id_;	
	GLint maxDrawBuffers_, maxVertexAttribs_;
	list<GLuint> vsIds_, gsIds_, fsIds_;
	string toStr_;
	bool linked_;
};

#endif
