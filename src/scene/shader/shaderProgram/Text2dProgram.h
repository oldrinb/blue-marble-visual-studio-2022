/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 14, 2021
 **/

#ifndef TEXT_2D_PROGRAM_H
#define TEXT_2D_PROGRAM_H

#include <GL/gl3w.h>

#include "BaseProgram.h"
#include "mesh/text/Text2D.h"

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



class Text2dProgram : public BaseProgram {
public:
	enum class ProgramMode { TEXT_2D = 1u };


	Text2dProgram();
	virtual ~Text2dProgram();


	// init: link, setAttribPointers (for each object 'pText2D')
	//############################################################################
	void link(Text2dProgram::ProgramMode programMode, list<GLuint>& rVertexShaderList, list<GLuint>& rFragmentShaderList);

	void setAttribPointers(Text2D* pText2D) const;
		

	// render (object 'pkText2D'): 1) start
	//                             2) [set...]
	//                             3) [render]
	//                             4) stop
	//############################################################################
	virtual void start() const;
	//-> void stop() const;

	void render(const Text2D* pkText2D) const;

	void setWindowSize(const GLuint* pkSize) const;
		
	void setTextMode(GLuint inverseColor) const;

	void setTextureUnit(GLint textureUnit) const;


	// get
	//############################################################################
	//-> bool isLinked() const;
	//-> bool isInstalled() const;

private:
	Text2dProgram(const Text2dProgram&);
	const Text2dProgram& operator=(const Text2dProgram&) {}

	void bindAttribLocations_(Text2dProgram::ProgramMode programMode);
	void bindOutputLocations_(Text2dProgram::ProgramMode programMode);
	void queryUniformLocations_(Text2dProgram::ProgramMode programMode);

	GLint shdWindowSize_, shdInverseColor_, shdTexSampler_;
};

#endif
