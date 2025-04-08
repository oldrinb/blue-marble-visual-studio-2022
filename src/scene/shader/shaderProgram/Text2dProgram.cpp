/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 14, 2021
 **/

#include "Text2dProgram.h"



Text2dProgram::Text2dProgram(): BaseProgram(), shdWindowSize_(-1), shdInverseColor_(-1), shdTexSampler_(-1)  {
	//cout << "Text 2D program created." << endl;
}



Text2dProgram::~Text2dProgram() {
	//cout << "Text 2D program deleted." << endl;
}



void Text2dProgram::link(Text2dProgram::ProgramMode programMode, 
	                     list<GLuint>& rVertexShaderList, list<GLuint>& rFragmentShaderList) {
	try {
		bindAttribLocations_(programMode);
		bindOutputLocations_(programMode);

		pProgram_->link(rVertexShaderList, rFragmentShaderList);

		queryUniformLocations_(programMode);
	}
	catch (const exception& kException) {
		throw runtime_error("Text2dProgram.link > " + string(kException.what()));
	}
}



void Text2dProgram::setAttribPointers(Text2D* pText2D) const {
	pText2D->setAttribPointers(pProgram_->getId(), BaseProgram::getVerticesAttribLocation_(),
		                       BaseProgram::getTexCoordsAttribLocation_(), BaseProgram::getColorsAttribLocation_());
}



void Text2dProgram::start() const {
	if (pProgram_->isLinked())
		BaseProgram::start();
	else throw runtime_error("Text2dProgram.start|Text 2d program " + to_string(pProgram_->getId()) + " not linked.");
}



void Text2dProgram::render(const Text2D* pkText2D) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled())
			try {
				pkText2D->render(pProgram_->getId());
			}
			catch (const exception& kException) {
				throw runtime_error("Text2dProgram.render > " + string(kException.what()));
			}
		else throw runtime_error("Text2dProgram.render|Text 2d program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("Text2dProgram.render|Text 2d program " + to_string(pProgram_->getId()) + " not linked.");
}



void Text2dProgram::setWindowSize(const GLuint* pkSize) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled())
			pProgram_->setUniformVector2ui(shdWindowSize_, pkSize);
		else throw 
			runtime_error("Text2dProgram.setWindowSize|Text 2d program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("Text2dProgram.setWindowSize|Text 2d program " + to_string(pProgram_->getId()) + " not linked.");
}



void Text2dProgram::setTextMode(GLuint inverseColor) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled())
			pProgram_->setUniformui(shdInverseColor_, inverseColor);
		else throw runtime_error("Text2dProgram.setTextMode|Text 2d program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("Text2dProgram.setTextMode|Text 2d program " + to_string(pProgram_->getId()) + " not linked.");
}



void Text2dProgram::setTextureUnit(GLint textureUnit) const {
	if (pProgram_->isLinked())
		if (pProgram_->isInstalled())
			pProgram_->setUniformi(shdTexSampler_, textureUnit);
		else throw 
			runtime_error("Text2dProgram.setTextureUnit|Text 2d program " + to_string(pProgram_->getId()) + " not installed.");
	else throw runtime_error("Text2dProgram.setTextureUnit|Text 2d program " + to_string(pProgram_->getId()) + " not linked.");
}



void Text2dProgram::bindAttribLocations_(Text2dProgram::ProgramMode) {
	try {
		pProgram_->setAttribLocation("vPosition", BaseProgram::getVerticesAttribLocation_());
		pProgram_->setAttribLocation("vTexCoord", BaseProgram::getTexCoordsAttribLocation_());
		pProgram_->setAttribLocation("vColor", BaseProgram::getColorsAttribLocation_());		
	}
	catch (const exception& kException) {
		throw runtime_error("Text2dProgram.bindAttribLocations_ > " + string(kException.what()));
	}
}



void Text2dProgram::bindOutputLocations_(Text2dProgram::ProgramMode) {
	try {
		pProgram_->setOutputLocation("outputColor", BaseProgram::getOutputFragLocation_());
	}
	catch (const exception& kException) {
		throw runtime_error("Text2dProgram.bindOutputLocations_ > " + string(kException.what()));
	}
}



void Text2dProgram::queryUniformLocations_(Text2dProgram::ProgramMode) {
	try {
		shdWindowSize_ = pProgram_->getUniformLocation("windowSize");
		shdInverseColor_ = pProgram_->getUniformLocation("inverseColor");
		shdTexSampler_ = pProgram_->getUniformLocation("texSampler");
	}
	catch (const exception& kException) {
		throw runtime_error("Text2dProgram.queryUniformLocations_ > " + string(kException.what()));
	}
}
