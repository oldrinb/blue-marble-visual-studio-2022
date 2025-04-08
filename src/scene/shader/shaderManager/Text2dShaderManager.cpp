/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 14, 2021
 **/

#include "Text2dShaderManager.h"



GLint Text2dShaderManager::getTextureUnit() {
	return 0;
}



Text2dShaderManager::Text2dShaderManager(const uvec2& kWindowSize): BaseShaderManager(kWindowSize), pText2dProgram_(nullptr) {
	try {
		pText2dProgram_ = new Text2dProgram();
	}
	catch (const exception& kException) {
		throw runtime_error("Text2dShaderManager > " + string(kException.what()));
	}

	nPrograms_ = 1u;
	ppPrograms_ = new BaseProgram*[nPrograms_];
	ppPrograms_[0u] = pText2dProgram_;

	pCurrentProgram_ = pText2dProgram_;

	//cout << "Text 2D shader manager created." << endl;	
}



Text2dShaderManager::~Text2dShaderManager() {
	delete pText2dProgram_;

	//cout << "Text 2D shader manager deleted." << endl;
}



void Text2dShaderManager::addShaderSourceCode(Text2dProgram::ProgramMode programMode, const string& kPath,
	                                          const list<string>& kVertexShaderFileList, const string& kVertexShaderHeader,
	                                          const list<string>& kFragmentShaderFileList, const string& kFragmentShaderHeader) {
	try {
		addVertexShaderSourceCode(programMode, kPath, kVertexShaderFileList, kVertexShaderHeader);
		addFragmentShaderSourceCode(programMode, kPath, kFragmentShaderFileList, kFragmentShaderHeader);
	}
	catch (const exception& kException) {
		throw runtime_error("Text2dShaderManager.addShaderSourceCode > " + string(kException.what()));
	}
}



void Text2dShaderManager::addVertexShaderSourceCode(Text2dProgram::ProgramMode programMode, const string& kPath,
	                                                const list<string>& kFileList, const string& kHeader) {
	try {
		BaseShaderManager::addVertexShaderSourceCode_(static_cast<BaseProgram::ProgramMode>(programMode),
			                                          kPath, kFileList, kHeader);
	}
	catch (const exception& kException) {
		throw runtime_error("Text2dShaderManager.addVertexShaderSourceCode > " + string(kException.what()));
	}
}



void Text2dShaderManager::addFragmentShaderSourceCode(Text2dProgram::ProgramMode programMode, const string& kPath,
	                                                  const list<string>& kFileList, const string& kHeader) {
	try {
		BaseShaderManager::addFragmentShaderSourceCode_(static_cast<BaseProgram::ProgramMode>(programMode),
			                                            kPath, kFileList, kHeader);
	}
	catch (const exception& kException) {
		throw runtime_error("Text2dShaderManager.addFragmentShaderSourceCode > " + string(kException.what()));
	}
}



void Text2dShaderManager::linkProgram(Text2dProgram::ProgramMode programMode) {
	try {
		list<GLuint> vertexShaderList, fragmentShaderList;
		BaseShaderManager::linkProgram_(static_cast<BaseProgram::ProgramMode>(programMode), vertexShaderList, fragmentShaderList);

		switch (programMode) {
		case Text2dProgram::ProgramMode::TEXT_2D:
			pText2dProgram_->link(programMode, vertexShaderList, fragmentShaderList);
			break;
		default:
			return;
		}

		pText2dProgram_->start();
		pText2dProgram_->setWindowSize(value_ptr(windowSize_));

		pText2dProgram_->setTextureUnit(Text2dShaderManager::getTextureUnit());
		pText2dProgram_->stop();
	}
	catch (const exception& kException) {
		throw runtime_error("Text2dShaderManager.linkProgram > " + string(kException.what()));
	}
}



void Text2dShaderManager::setAttribPointers(Text2D* pText2D) const {
	pText2dProgram_->setAttribPointers(pText2D);
}



void Text2dShaderManager::setWindowSize(const uvec2& kSize) {
	BaseShaderManager::setWindowSize(kSize);

	try {
		pText2dProgram_->start();
		pText2dProgram_->setWindowSize(value_ptr(kSize));
		pText2dProgram_->stop();
	}
	catch (const exception& kException) {
		throw runtime_error("Text2dShaderManager.setWindowSize > " + string(kException.what()));
	}
}



void Text2dShaderManager::startProgram(Text2dProgram::ProgramMode programMode) {
	switch (programMode) {
	case Text2dProgram::ProgramMode::TEXT_2D:
		break;
	default:
		throw runtime_error("Text2dShaderManager.startProgram|Invalid program mode value.");
	}
	
	try {
		pText2dProgram_->start();
	}
	catch (const exception& kException) {
		throw runtime_error("Text2dShaderManager.startProgram > " + string(kException.what()));
	}
}



void Text2dShaderManager::render(const Text2D* pkText2D) const {
	try {
		pText2dProgram_->render(pkText2D);
	}
	catch (const exception& kException) {
		throw runtime_error("Text2dShaderManager.render > " + string(kException.what()));
	}
}



void Text2dShaderManager::setTextParameters(bool inverseColor) const {
	try {
		pText2dProgram_->setTextMode(static_cast<GLuint>(inverseColor));
	}
	catch (const exception& kException) {
		throw runtime_error("Text2dShaderManager.setTextParameters > " + string(kException.what()));
	}
}
