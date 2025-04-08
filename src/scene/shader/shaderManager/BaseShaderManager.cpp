/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 31, 2025
 **/

#include "BaseShaderManager.h"



BaseShaderManager::~BaseShaderManager() {
	delete[] ppPrograms_;

	//cout << "[Base shader manager] deleted." << endl;
}



void BaseShaderManager::compileShaders() {
	try {
		for (Shader& iShader : vertexShaders_)
			iShader.compile();
		for (Shader& iShader : fragmentShaders_)
			iShader.compile();
	}
	catch (const exception& kException) {
		throw runtime_error("BaseShaderManager.compileShaders > " + string(kException.what()));
	}
}



void BaseShaderManager::setWindowSize(const uvec2& kSize) {
	if (kSize.x == 0u || kSize.y == 0u)
		throw runtime_error("BaseShaderManager.setWindowSize|Invalid window size value.");
	windowSize_ = kSize;
}



void BaseShaderManager::stopProgram() const {
	pCurrentProgram_->stop();
}



BaseShaderManager::BaseShaderManager(const uvec2& kWindowSize): ppPrograms_(nullptr), pCurrentProgram_(nullptr), nPrograms_(0u),
                                     windowSize_(kWindowSize), vertexShaders_(), fragmentShaders_(),
                                     vertexShaderRef_(), fragmentShaderRef_() {
	if (kWindowSize.x == 0u || kWindowSize.y == 0u)
		throw runtime_error("BaseShaderManager|Invalid window size value.");

	//cout << "[Base shader manager] created." << endl;
}



void BaseShaderManager::addVertexShaderSourceCode_(BaseProgram::ProgramMode programMode,
	                                               const string& kPath, const list<string>& kFileList, const string& kHeader) {
	try {
		Shader* pShader = nullptr;
		for (Shader& iShader : vertexShaders_)
			if (iShader.getPath() == kPath && iShader.getFileList() == kFileList && iShader.getHeader() == kHeader) {
				pShader = &iShader;
				break;
			}

		if (!pShader) {
			vertexShaders_.emplace_back(GL_VERTEX_SHADER);
			pShader = &vertexShaders_.back();
			pShader->readSourceCode(kPath, kFileList, kHeader);
		}
		if (pShader) vertexShaderRef_.push_back(std::make_pair(programMode, pShader));
	}
	catch (const exception& kException) {
		throw runtime_error("BaseShaderManager.addVertexShaderSourceCode_ > " + string(kException.what()));
	}
}



void BaseShaderManager::addFragmentShaderSourceCode_(BaseProgram::ProgramMode programMode,
	                                                 const string& kPath, const list<string>& kFileList, const string& kHeader) {
	try {
		Shader* pShader = nullptr;
		for (Shader& iShader : fragmentShaders_)
			if (iShader.getPath() == kPath && iShader.getFileList() == kFileList && iShader.getHeader() == kHeader) {
				pShader = &iShader;
				break;
			}

		if (!pShader) {
			fragmentShaders_.emplace_back(GL_FRAGMENT_SHADER);
			pShader = &fragmentShaders_.back();
			pShader->readSourceCode(kPath, kFileList, kHeader);
		}
		if (pShader) fragmentShaderRef_.push_back(std::make_pair(programMode, pShader));
	}
	catch (const exception& kException) {
		throw runtime_error("BaseShaderManager.addFragmentShaderSourceCode_ > " + string(kException.what()));
	}
}



void BaseShaderManager::linkProgram_(BaseProgram::ProgramMode programMode, 
	                                 list<GLuint>& rVertexShaderList, list<GLuint>& rFragmentShaderList) {
	for (const pair<BaseProgram::ProgramMode, Shader*> ikRef : vertexShaderRef_)
		if (ikRef.first == programMode) {
			const Shader* pkShader = ikRef.second;
			GLuint id = pkShader->getId();
			if (pkShader->isCompiled())
				rVertexShaderList.push_back(id);
			else throw runtime_error("BaseShaderManager.linkProgram_|Vertex shader " + to_string(id) + " not compiled.");
		}

	for (const pair<BaseProgram::ProgramMode, Shader*> ikRef : fragmentShaderRef_)
		if (ikRef.first == programMode) {
			const Shader* pkShader = ikRef.second;
			GLuint id = pkShader->getId();
			if (pkShader->isCompiled())
				rFragmentShaderList.push_back(id);
			else throw runtime_error("BaseShaderManager.linkProgram_|Fragment shader " + to_string(id) + " not compiled.");
		}
}
