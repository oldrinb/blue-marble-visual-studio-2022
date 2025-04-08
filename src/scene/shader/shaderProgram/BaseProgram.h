/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 14, 2021
 **/

#ifndef BASE_PROGRAM_H
#define BASE_PROGRAM_H

#include <GL/gl3w.h>

#include "shader/program/Program.h"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

using std::cout;
using std::endl;
using std::exception;
using std::runtime_error;
using std::string;



class BaseProgram {
public:
	enum class ProgramMode { NONE = 0u };

	virtual ~BaseProgram();	

	virtual void start() const;
	void stop() const;

	bool isLinked() const;
	bool isInstalled() const;

protected:
	static GLuint getVerticesAttribLocation_();
	static GLuint getTexCoordsAttribLocation_();
	static GLuint getColorsAttribLocation_();
	static GLuint getNormalsAttribLocation_();
	static GLuint getTangentsAttribLocation_();
	static GLuint getBitangentsAttribLocation_();

	static GLuint getOutputFragLocation_();

	BaseProgram();

	Program* pProgram_;

private:
	BaseProgram(const BaseProgram&);
	const BaseProgram& operator=(const BaseProgram&) {}
};
#endif
