/**
 * Author: Oldrin Barbulescu
 * Last modified: Feb 16, 2021
 **/

#include "ColorTexture.h"



ColorTexture::ColorTexture(GLsizei width, GLsizei height, bool repeat, bool linearFiltering): BaseTexture() {
	width_ = width; 
	height_ = height;
	internalFormat_ = GL_RGBA8;
	format_ = GL_RGBA;
	dataType_ = GL_UNSIGNED_BYTE;
	nMipmaps_ = 1;

	bool mipmapping = false;
	bool compareMode = false;

	try {
		initTexture_(mipmapping);
		setTexParameters_(repeat, linearFiltering, mipmapping, compareMode);
		generateMipmaps_(mipmapping);
	}
	catch (const exception& kException) {
		throw runtime_error("ColorTexture > " + string(kException.what()));
	}

	//cout << "Color texture " << id_ << " created (1st constructor)." << endl;
}



ColorTexture::ColorTexture(const string& kFilePath, const string& kFileName, 
	                       bool repeat, bool linearFiltering, bool mipmapping) : BaseTexture() {
	ppTextureData_ = new GLubyte*[1u];
	ppTextureData_[0u] = nullptr;
	
	size_t size = kFileName.length();
	if (size < 5u)
		throw runtime_error("ColorTexture|Image name too short.");
	string extension = kFileName.substr(size - 4u, 4u);
	dds_ = (extension == ".DDS" || extension == ".dds");
	
	try {
		if (dds_) loadDDsImage_(kFilePath, kFileName, false, false, 0u);
		else loadIlImage_(kFilePath, kFileName, false, 0u);

		bool compareMode = false;

		initTexture_(mipmapping);
		setTexParameters_(repeat, linearFiltering, mipmapping, compareMode);
		generateMipmaps_(mipmapping);
		computeTexTransparency_();
	}
	catch (const exception& kException) {
		throw runtime_error("ColorTexture > " + string(kException.what()));
	}

	//cout << "Color texture " << id_ << " created (2nd constructor)." << endl;
}



ColorTexture::~ColorTexture() {
	//cout << "Color texture " << id_ << " deleted." << endl;
}



string ColorTexture::getFilePath() const {
	return filePath_;
}



string ColorTexture::getFileName() const {
	return fileName_;
}



bool ColorTexture::isDDS() const {
	return dds_;
}



bool ColorTexture::isTransparent() const {
	return transparent_;
}



void ColorTexture::initTexture_(bool mipmapping) {
	BaseTexture::initTexture_(mipmapping);

	glBindTexture(GL_TEXTURE_2D, id_);

	if (ppTextureData_ && ppTextureData_[0u]) {
		if (dds_ && pOffsets_) {
			GLint level = 0;
			GLsizei tempWidth = width_, tempHeight = height_;

			do {
				if (level < nMipmaps_)
					setTexImage_(level, max(1, tempWidth), max(1, tempHeight), pOffsets_[level + 1], 
						         ppTextureData_[0u] + pOffsets_[level]);
				else setTexImage_(level, max(1, tempWidth), max(1, tempHeight), 0, nullptr);

				tempWidth /= 2; tempHeight /= 2; level++;
			} while (mipmapping && nMipmaps_ > 1 && (tempWidth > 0 || tempHeight > 0));

			delete[] pOffsets_; pOffsets_ = nullptr;

			//cout << endl << "DDS texture '" << fileName_ << "' loaded." << endl;
		}
		else if (!dds_) {
			setTexImage_(0, width_, height_, 0, ppTextureData_[0u]);

			//cout << endl << "Texture '" << fileName_ << "' loaded." << endl;
		}

		delete[] ppTextureData_[0u]; ppTextureData_[0u] = nullptr;
		delete[] ppTextureData_; ppTextureData_ = nullptr;
	}
	else setTexImage_(0, width_, height_, 0, nullptr);

	glBindTexture(GL_TEXTURE_2D, 0u);
}
