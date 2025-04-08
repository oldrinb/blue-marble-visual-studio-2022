/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 14, 2021
 **/

#include "Shader.h"



Shader::Shader(GLenum shaderType): id_(0u), shaderType_ (shaderType), path_(), header_(), toStr_(), fileList_(),
                                   ppSource_(nullptr), nFiles_(0), compiled_(false) {
	switch (shaderType) {
		case GL_VERTEX_SHADER:
		case GL_GEOMETRY_SHADER:
		case GL_FRAGMENT_SHADER:
			id_ = glCreateShader(shaderType);
			if (id_ == 0u) throw runtime_error("Shader|Create shader failed.");
			break;
		default:
			throw runtime_error("Shader|Invalid shader type value.");
	}

	//cout << getType_() << " " << id_ << " created." << endl;
}



Shader::~Shader() {
	if (ppSource_) {
		for (GLsizei i = 0; i < nFiles_; i++) 
			if (ppSource_[i]) delete[] ppSource_[i];
		delete[] ppSource_;
	}

	glDeleteShader(id_);

	//cout << getType_() << " " << id_ << " deleted." << endl;
}



void Shader::readSourceCode(const string& kPath, const list<string>& kFileList, const string& kHeader) {
	if (ppSource_)
		throw runtime_error("Shader.readSourceCode|" + getType_() + " " + to_string(id_) + " source file(s) already loaded.");

	nFiles_ = static_cast<GLsizei>(kFileList.size());
	if (nFiles_ == 0)
		throw runtime_error("Shader.readSourceCode|File name list cannot be empty.");
	path_ = kPath;
	fileList_ = kFileList;
	header_ = kHeader;
	toStr_ = toString_();

	if (!kHeader.empty()) nFiles_++;
	ppSource_ = new GLchar*[static_cast<size_t>(nFiles_)];
	for (GLsizei i = 0; i < nFiles_; i++)
		ppSource_[i] = nullptr;

	if (!kHeader.empty()) {
		ppSource_[0u] = new GLchar[kHeader.size() + 1u];
		kHeader.copy(ppSource_[0u], kHeader.size());
		ppSource_[0u][kHeader.size()] = 0u;
	}	

	list<string>::const_iterator ikFile;
	GLsizei i = kHeader.empty() ? 0 : 1;
	for (ikFile = kFileList.begin(); ikFile != kFileList.end() && i < nFiles_; ikFile++, i++) {
		try {
			if (kPath.empty()) 
				readFile_(ikFile->c_str(), static_cast<unsigned int>(i));			
			else readFile_((kPath + "/" + *ikFile).c_str(), static_cast<unsigned int>(i));
		}
		catch (const exception& kException) {
			throw runtime_error("Shader.readSourceCode > " + string(kException.what()));
		}
	}
}



void Shader::compile() {
	if (!ppSource_)
		throw runtime_error("Shader.compile|" + getType_() + " " + to_string(id_) + " source file(s) not loaded yet.");
	if (compiled_)
		throw runtime_error("Shader.compile|" + getType_() + " " + to_string(id_) + " already compiled.");

	glShaderSource(id_, nFiles_, const_cast<const GLchar**> (ppSource_), NULL);
	glCompileShader(id_);

	GLint compiledStatus;
	glGetShaderiv(id_, GL_COMPILE_STATUS, &compiledStatus);
	if (compiledStatus == GL_FALSE) {
		string errorMessage = "Shader.compile|Shader compilation failed. ";
		GLint len;
		glGetShaderiv(id_, GL_INFO_LOG_LENGTH, &len);
		if (len > 0) {
			GLchar* pLog = new GLchar[static_cast<size_t>(len)];
			glGetShaderInfoLog(id_, len, &len, pLog);
			errorMessage += pLog;
			delete[] pLog;
		}
		throw runtime_error(errorMessage);
	}

	compiled_ = true;
	//cout << getType_() << " " << id_ << " succesfully compiled." << endl;
}



GLenum Shader::getShaderType() const {
	return shaderType_;
}



string Shader::getPath() const {
	if (ppSource_) return path_;
	else throw runtime_error("Shader.getPath|" + getType_() + " " + to_string(id_) + " source file(s) not loaded yet.");
}



list<string> Shader::getFileList() const {
	if (ppSource_) return fileList_;
	else throw runtime_error("Shader.getFileList|" + getType_() + " " + to_string(id_) + " source file(s) not loaded yet.");
}



string Shader::getHeader() const {
	if (ppSource_) return header_;
	else throw runtime_error("Shader.getHeader|" + getType_() + " " + to_string(id_) + " source file(s) not loaded yet.");
}



bool Shader::isCompiled() const {
	return compiled_;
}



GLuint Shader::getId() const {
	return id_;
}



string Shader::toString() const {
	if (ppSource_) return toStr_;
	else throw runtime_error("Shader.toString|" + getType_() + " " + to_string(id_) + " source file(s) not loaded yet.");	
}



void Shader::readFile_(const char* pkFileName, unsigned int fileNo) {
	FILE* pFile;
	fopen_s(&pFile, pkFileName, "rb");

	if (pFile == NULL)
		throw runtime_error("Shader.readFile_|Cannot read the '" + string(pkFileName) + "' file.");	

	int success = std::fseek(pFile, 0L, SEEK_END);
	if (success != 0) {
		std::fclose(pFile);
		throw runtime_error("Shader.readFile_|An error occured while reading the '" + string(pkFileName) + "' file.");
	}

	long int len = std::ftell(pFile);
	if (len == -1L) {
		std::fclose(pFile);
		throw runtime_error("Shader.readFile_|An error occured while reading the '" + string(pkFileName) + "' file.");
	}
	
	success = std::fseek(pFile, 0L, SEEK_SET);
	if (success != 0) {
		std::fclose(pFile);
		throw runtime_error("Shader.readFile_|An error occured while reading the '" + string(pkFileName) + "' file.");
	}

	ppSource_[fileNo] = new GLchar[static_cast<size_t>(len) + 1];

	size_t count = std::fread(ppSource_[fileNo], 1u, static_cast<size_t>(len), pFile);
	if (count != static_cast<size_t>(len)) {
		std::fclose(pFile);
		throw runtime_error("Shader.readFile_|An error occured while reading the '" + string(pkFileName) + "' file.");
	}

	ppSource_[fileNo][len] = 0u;

	std::fclose(pFile);
}



string Shader::getType_() const {
	switch (shaderType_) {
	case GL_VERTEX_SHADER:
		return "Vertex shader";
		break;
	case GL_GEOMETRY_SHADER:
		return "Geometry shader";
		break;
	case GL_FRAGMENT_SHADER:
		return "Fragment shader";
		break;
	default:
		return "";
	}
}



string Shader::toString_() const {
	string result = to_string(id_) + " ";
	if (!header_.empty()) result += "(" + header_ + ")";

	list<string>::const_iterator ikFile;
	for (ikFile = fileList_.begin(); ikFile != fileList_.end(); ikFile++) {
		if (ikFile != fileList_.begin())
			result += "|";

		if (path_.empty())
			result += *ikFile;
		else result += path_ + "/" + *ikFile;
	}

	size_t pos = result.find("\n");
	while (pos != string::npos) {
		result.replace(pos, 1u, "");
		pos = result.find("\n", pos + 1u);
	}

	return result;
}
