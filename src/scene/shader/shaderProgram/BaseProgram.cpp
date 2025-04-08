/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 13, 2021
 **/

#include "BaseProgram.h"



GLuint BaseProgram::getVerticesAttribLocation_() {
	return 0u;
}



GLuint BaseProgram::getTexCoordsAttribLocation_() {
	return 1u;
}



GLuint BaseProgram::getColorsAttribLocation_() {
	return 2u;
}



GLuint BaseProgram::getNormalsAttribLocation_() {
	return 3u;
}



GLuint BaseProgram::getTangentsAttribLocation_() {
	return 4u;
}



GLuint BaseProgram::getBitangentsAttribLocation_() {
	return 5u;
}



GLuint BaseProgram::getOutputFragLocation_() {
	return 0u;
}



BaseProgram::~BaseProgram() {
	delete pProgram_;

	//cout << "[Base program] deleted." << endl;
}



void BaseProgram::start() const {
	pProgram_->start();
}



void BaseProgram::stop() const {
	pProgram_->stop();
}



bool BaseProgram::isLinked() const {
	return pProgram_->isLinked();
}



bool BaseProgram::isInstalled() const {
	return pProgram_->isInstalled();
}



BaseProgram::BaseProgram(): pProgram_(nullptr) {
	try {
		pProgram_ = new Program();
	}
	catch (const exception& kException) {
		throw runtime_error("BaseProgram > " + string(kException.what()));
	}

	//cout << "[Base program] created." << endl;
}
