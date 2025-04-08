/**
 * Author: Oldrin Barbulescu
 * Last modified: Feb 3, 2025
 **/

#include "BaseTexture.h"



const unsigned int BaseTexture::knCubeFaces_ = 6u;



BaseTexture::~BaseTexture() {
	glDeleteTextures(1, &id_);

	if (ppTextureData_) {
		if (ppTextureData_[0u]) delete[] ppTextureData_[0u];
		delete[] ppTextureData_;
	}

	if (pOffsets_) delete[] pOffsets_;

	//cout << "[Base texture " << id_ << "] deleted." << endl;
}



void BaseTexture::startReading(GLint textureUnit) const {
	if (textureUnit >= 0 && textureUnit < maxCombinedTextureImageUnits_) {
		glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(textureUnit));
		glBindTexture(GL_TEXTURE_2D, id_);
	}
	else throw runtime_error("BaseTexture.startReading|Invalid texture unit value.");
}



void BaseTexture::stopReading() const {
	glBindTexture(GL_TEXTURE_2D, 0u);
}



GLsizei BaseTexture::getWidth() const {
	return width_;
}



GLsizei BaseTexture::getHeight() const {
	return height_;
}



GLuint BaseTexture::getId() const {
	return id_;
}



BaseTexture::BaseTexture() : ppTextureData_(nullptr), pOffsets_(nullptr), filePath_(), fileName_(), width_(0), height_(0),
                             nMipmaps_(0), internalFormat_(0), maxCombinedTextureImageUnits_(0), format_(0u),  dataType_(0u),
	                         dds_(false), compressed_(false), transparent_(false), id_(0u) {
	glGenTextures(1, &id_);
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedTextureImageUnits_);

	//cout << "[Base texture " << id_ << "] created." << endl;
}



void BaseTexture::loadIlImage_(const string& kFilePath, const string& kFileName, bool cubeMap, GLuint face) {
	ILuint imageName = 0u;
	ilGenImages(1, &imageName);
	if (imageName == 0u)
		throw runtime_error("BaseTexture.loadIlImage_|Create DevIL image name failed.");

	ilBindImage(imageName);
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	ILboolean succes = ilLoadImage((const wchar_t *)((kFilePath + "/" + kFileName).c_str()));

	if (succes) {
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

		GLint kInternalFormat = GL_SRGB8_ALPHA8;
		GLenum kFormat = GL_RGBA;
		GLenum kDataType = GL_UNSIGNED_BYTE;

		ILint width = ilGetInteger(IL_IMAGE_WIDTH);
		ILint height = ilGetInteger(IL_IMAGE_HEIGHT);

		bool cubeDDS = false;
		bool compressed = false;
		GLint nMipmaps = 1;		

		try {
			setTextureParameters_(kFilePath, kFileName, cubeMap, cubeDDS, face, 
				                  width, height, nMipmaps, kInternalFormat, kFormat, kDataType, compressed);
		}
		catch (const exception& kException) {
			ilBindImage(0u);
			ilDeleteImages(1, &imageName);
			throw runtime_error("BaseTexture.loadIlImage_ > " + string(kException.what()));
		}

		ILubyte* pBytes = ilGetData();
		ILint size = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);		

		ppTextureData_[face] = new GLubyte[static_cast<size_t>(size)];
		for (ILint i = 0; i < size; i++)
			ppTextureData_[face][i] = pBytes[i];
	}

	ilBindImage(0u);
	ilDeleteImages(1, &imageName);
	if (!succes) throw runtime_error("BaseTexture.loadIlImage_|Cannot load image '" + kFileName + "'");
}



void BaseTexture::loadDDsImage_(const string& kFilePath, const string& kFileName, bool cubeMap, bool cubeDDS, GLuint face) {
	FILE* pFile;
	fopen_s(&pFile, string(kFilePath + "/" + kFileName).c_str(), "rb");
	if (pFile == NULL) 
		throw runtime_error("BaseTexture.loadDDsImage_|Cannot read '" + kFileName + "' file.");

	GLenum kDataType = GL_UNSIGNED_BYTE;

	long int fileLength;
	GLsizei width, height;
	GLint nMipmaps, internalFormat;
	GLenum format;
	GLuint blockSize, caps2, nFaces;
	bool compressed;

	GLuint* pPixelFormat = nullptr;

	try {
		readDDsHeader_(pFile, kFileName, fileLength, height, width, nMipmaps, pPixelFormat, caps2);
		
		getTextureFormat_(pPixelFormat, internalFormat, format, blockSize, compressed);
		setTextureParameters_(kFilePath, kFileName, cubeMap, cubeDDS, face, 
			                  width, height, nMipmaps, internalFormat, format, kDataType, compressed);

		getNumFaces_(caps2, cubeDDS, nFaces);

		delete[] pPixelFormat;
	}
	catch (const exception& kException) {
		if (pPixelFormat) delete[] pPixelFormat;
		std::fclose(pFile);
		throw runtime_error("BaseTexture.loadDDsImage_ > " + string(kException.what()));
	}
	
	pOffsets_ = new GLsizei[static_cast<size_t>(nMipmaps) + 1u];
	GLsizei offset = 0; GLint level = 0;

	do {
		pOffsets_[level] = offset;
		if (compressed)
			offset += max(1, ((width + 3) / 4)) * max(1, ((height + 3) / 4)) * blockSize;
		else offset += (width * blockSize + 7) / 8 * height;

		width /= 2; height /= 2; level++;
	} while (level < nMipmaps && (width > 0 || height > 0));

	pOffsets_[nMipmaps] = offset;

	if (fileLength >= static_cast<long int>(nFaces) * offset) {
		if (cubeDDS)
			for (GLuint iFace = 0u; iFace < nFaces; iFace++) {
				ppTextureData_[iFace] = new GLubyte[static_cast<size_t>(offset)];
				size_t count = std::fread(ppTextureData_[iFace], 1u, static_cast<size_t>(offset), pFile);

				if (count != static_cast<size_t>(offset)) {
					std::fclose(pFile);
					throw runtime_error("BaseTexture.loadDDsImage_|An error occured while reading the '" + kFileName + "' file.");
				}
			}
		
		else {
			ppTextureData_[face] = new GLubyte[static_cast<size_t>(offset)];
			size_t count = std::fread(ppTextureData_[face], 1u, static_cast<size_t>(offset), pFile);

			if (count != static_cast<size_t>(offset)) {
				std::fclose(pFile);
				throw runtime_error("BaseTexture.loadDDsImage_|An error occured while reading the '" + kFileName + "' file.");
			}
		}

		std::fclose(pFile);
	}
	else {
		std::fclose(pFile);
		throw runtime_error("BaseTexture.loadDDsImage_|Invalid DDS file: " + kFileName);
	}
}



void BaseTexture::initTexture_(bool) {
	if (width_ <= 0 || height_ <= 0)
		throw runtime_error("BaseTexture.initTexture_|Invalid texture size value.");	
}



void BaseTexture::setTexParameters_(bool repeat, bool linearFiltering, bool mipmapping, bool compareMode) const {
	glBindTexture(GL_TEXTURE_2D, id_);

	if (repeat) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	if (linearFiltering) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if (mipmapping) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		if (mipmapping) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	if (compareMode) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);		
	}
	else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

	glBindTexture(GL_TEXTURE_2D, 0u);
}



void BaseTexture::generateMipmaps_(bool mipmapping) {
	glBindTexture(GL_TEXTURE_2D, id_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);

	if (mipmapping && nMipmaps_ == 1) {
		nMipmaps_ = 1 + static_cast<GLint>(std::floor(std::log2(max(width_, height_))));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, nMipmaps_ - 1);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else if (mipmapping && nMipmaps_ > 1) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, nMipmaps_ - 1);
	else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

	glBindTexture(GL_TEXTURE_2D, 0u);
}



void BaseTexture::computeTexTransparency_() {
	glBindTexture(GL_TEXTURE_2D, id_);

	GLint textureWidth = 0, textureHeight = 0;

	for (GLint iLevel = 0; iLevel < nMipmaps_; iLevel++) {
		glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel, GL_TEXTURE_WIDTH, &textureWidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, iLevel, GL_TEXTURE_HEIGHT, &textureHeight);

		GLubyte* pPixels = new GLubyte[static_cast<size_t>(textureWidth) * textureHeight * 4u];
		glGetTexImage(GL_TEXTURE_2D, iLevel, GL_RGBA, GL_UNSIGNED_BYTE, pPixels);

		for (GLint row = 0; row < textureHeight; row++)
			for (GLint col = 0; col < textureWidth; col++) {
				GLubyte color = pPixels[((row * textureWidth) + col) * 4u + 3u];

				if (color < 255u) {
					delete[] pPixels;
					glBindTexture(GL_TEXTURE_2D, 0u);
					transparent_ = true;
					return;
				}
			}

		delete[] pPixels;
	}

	glBindTexture(GL_TEXTURE_2D, 0u);
}



void BaseTexture::setTexImage_(GLint level, GLsizei width, GLsizei height, GLsizei compressedSize,
	                           const GLubyte* pkTextureData) const {
	if (compressed_)
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format_, width, height, 0, compressedSize,
		                       (pkTextureData) ? pkTextureData : NULL);

	else glTexImage2D(GL_TEXTURE_2D, level, internalFormat_, width, height, 0, format_, dataType_, 
		              (pkTextureData) ? pkTextureData : NULL);
}



void BaseTexture::readDDsHeader_(FILE* pFile, const string& kFileName, long int& rFileLength, GLsizei& rHeight, GLsizei& rWidth,
	                             GLint& rnMipmaps, GLuint*& rpPixelFormat, GLuint& rCaps2) const {
	const GLuint DDS_MAGIC = 0x20534444;
	const GLsizei kMagicLength = 4, kHeaderLength = 124, kPixelFormatLength = 32;

	int success = std::fseek(pFile, 0L, SEEK_END);
	if (success != 0)
		throw runtime_error("BaseTexture.readDDsHeader_|An error occured while reading the '" + kFileName + "' file.");

	rFileLength = std::ftell(pFile);
	if (rFileLength == -1L)
		throw runtime_error("BaseTexture.readDDsHeader_|An error occured while reading the '" + kFileName + "' file.");
	if (rFileLength <= static_cast<long int>(kMagicLength + kHeaderLength))
		throw runtime_error("BaseTexture.readDDsHeader_|Invalid DDS file: " + kFileName);
	rFileLength -= (kMagicLength + kHeaderLength);

	std::rewind(pFile);

	GLuint magic;
	size_t count = std::fread(&magic, 4u, kMagicLength / 4u, pFile);
	if (count != kMagicLength / 4u)
		throw runtime_error("BaseTexture.readDDsHeader_|An error occured while reading the '" + kFileName + "' file.");
	if (magic != DDS_MAGIC)
		throw runtime_error("BaseTexture.readDDsHeader_|Wrong type of file: " + kFileName);

	GLuint ddsHeader[kHeaderLength / 4u];
	count = std::fread(&ddsHeader, 4u, kHeaderLength / 4u, pFile);
	if (count != kHeaderLength / 4)
		throw runtime_error("BaseTexture.readDDsHeader_|An error occured while reading the '" + kFileName + "' file.");

	rHeight = static_cast<GLsizei>(ddsHeader[2u]);
	rWidth = static_cast<GLsizei>(ddsHeader[3u]);
	rnMipmaps = static_cast<GLint>(max(1u, ddsHeader[6u]));

	rpPixelFormat = new GLuint[kPixelFormatLength / 4u];

	try {
		std::copy(ddsHeader + 18u, ddsHeader + 18u + kPixelFormatLength / 4u, rpPixelFormat);
	}
	catch (const exception& kException) {
		throw runtime_error("BaseTexture.readDDsHeader_ > copy > " + string(kException.what()));
	}

	rCaps2 = ddsHeader[27u];
}



void BaseTexture::getTextureFormat_(const GLuint* pkPixelFormat, 
	                                GLint& rInternalFormat, GLenum& rFormat, GLuint& rBlockSize, bool& rCompressed) const {
	const GLuint DDPF_ALPHAPIXELS = 0x1;
	const GLuint DDPF_FOURCC = 0x4;
	const GLuint DDPF_RGB = 0x40;

	const GLuint FOURCC_DXT1 = 0x31545844;
	const GLuint FOURCC_DXT3 = 0x33545844;
	const GLuint FOURCC_DXT5 = 0x35545844;

	const GLuint kRedBitMask = 0x00ff0000;
	const GLuint kGreenBitMask = 0x0000ff00;
	const GLuint kBlueBitMask = 0x000000ff;
	const GLuint kAlfaBitMask = 0xff000000;

	GLuint flags = pkPixelFormat[1u];
	GLuint fourCC = pkPixelFormat[2u];
	GLuint rgbBitCount = pkPixelFormat[3u];
	GLuint redBitMask = pkPixelFormat[4u];
	GLuint greenBitMask = pkPixelFormat[5u];
	GLuint blueBitMask = pkPixelFormat[6u];
	GLuint alfaBitMask = pkPixelFormat[7u];

	if (flags & DDPF_FOURCC) {
		switch (fourCC) {
		case FOURCC_DXT1:
			rFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			//cout << "GL_COMPRESSED_RGBA_S3TC_DXT1_EXT" << endl;
			rBlockSize = 8u;
			break;
		case FOURCC_DXT3: // 'DXT3'
			rFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			//cout << "GL_COMPRESSED_RGBA_S3TC_DXT3_EXT" << endl;
			rBlockSize = 16u;
			break;
		case FOURCC_DXT5: // 'DXT5'
			rFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			//cout << "GL_COMPRESSED_RGBA_S3TC_DXT5_EXT" << endl;
			rBlockSize = 16u;
			break;
		default:
			throw runtime_error("BaseTexture.getTextureFormat_|DDS compressed format not supported.");
		}

		rInternalFormat = GL_COMPRESSED_RGBA;
		rCompressed = true;
	}

	else if ((flags & DDPF_RGB) && (redBitMask & kRedBitMask) && (greenBitMask & kGreenBitMask) && (blueBitMask & kBlueBitMask)) {
		if ((flags & DDPF_ALPHAPIXELS) && rgbBitCount == 32u && (alfaBitMask & kAlfaBitMask)) {
			rInternalFormat = GL_RGBA8;
			rFormat = GL_BGRA;
			//cout << "GL_BGRA" << endl;
			rBlockSize = 32u;
		}
		else if (rgbBitCount == 24u) {
			rInternalFormat = GL_RGB8;
			rFormat = GL_BGR;
			//cout << "GL_BGR" << endl;
			rBlockSize = 24u;
		}

		else throw runtime_error("BaseTexture.getTextureFormat_|DDS uncompressed format not supported.");

		rCompressed = false;
	}

	else throw runtime_error("BaseTexture.getTextureFormat_|Invalid DDS format or format not supported.");
}



void BaseTexture::getNumFaces_(GLuint caps2, bool cubeDDS, GLuint& rnFaces) const {
	const GLuint DDSCAPS2_CUBEMAP = 0x200;
	const GLuint DDSCAPS2_CUBEMAP_POSITIVEX = 0x400;
	const GLuint DDSCAPS2_CUBEMAP_NEGATIVEX = 0x800;
	const GLuint DDSCAPS2_CUBEMAP_POSITIVEY = 0x1000;
	const GLuint DDSCAPS2_CUBEMAP_NEGATIVEY = 0x2000;
	const GLuint DDSCAPS2_CUBEMAP_POSITIVEZ = 0x4000;
	const GLuint DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x8000;

	bool cubemap = (caps2 & DDSCAPS2_CUBEMAP);
	if (cubemap) {
		if ((caps2 & DDSCAPS2_CUBEMAP_POSITIVEX) &&
			(caps2 & DDSCAPS2_CUBEMAP_NEGATIVEX) &&
			(caps2 & DDSCAPS2_CUBEMAP_POSITIVEY) &&
			(caps2 & DDSCAPS2_CUBEMAP_NEGATIVEY) &&
			(caps2 & DDSCAPS2_CUBEMAP_POSITIVEZ) &&
			(caps2 & DDSCAPS2_CUBEMAP_NEGATIVEZ)) {

			if (cubeDDS) rnFaces = BaseTexture::knCubeFaces_;
			else throw runtime_error("BaseTexture.getNumFaces_|Wrong type of DDS file.");			
		}
		else throw runtime_error("BaseTexture.getNumFaces_|Incomplete cubemap faces.");		
	}

	else rnFaces = 1u;
}



void BaseTexture::setTextureParameters_(const string& kFilePath, const string& kFileName, bool cubeMap, bool cubeDDS, GLuint face,
	                                    GLsizei width, GLsizei height, GLint nMipmaps, 
	                                    GLint internalFormat, GLenum format, GLenum dataType, bool compressed) {
	if (!cubeMap || (cubeMap && !cubeDDS && face == 0u) || (cubeMap && cubeDDS)) {
		filePath_ = kFilePath;
		fileName_ = kFileName;
		width_ = width;
		height_ = height;
		nMipmaps_ = nMipmaps;
		internalFormat_ = internalFormat;
		format_ = format;
		dataType_ = dataType;
		compressed_ = compressed;

		if (cubeMap && width_ != height_)
			throw runtime_error("BaseTexture.setTextureParameters_|Cube map faces must be square.");
	}
	else if (cubeMap && !cubeDDS && face > 0u) {
		if (width != width_ || height != height_)
			throw runtime_error("BaseTexture.setTextureParameters_|Cube map faces must have the same size.");
		if (nMipmaps != nMipmaps_)
			throw runtime_error("BaseTexture.setTextureParameters_|Cube map faces must have the same number of mipmaps.");		
		if (format != format_)
			throw runtime_error("BaseTexture.setTextureParameters_|Cube map faces must have the same type of pixel format.");
	}
}
