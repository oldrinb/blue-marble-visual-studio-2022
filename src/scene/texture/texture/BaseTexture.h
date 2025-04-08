/**
 * Author: Oldrin Barbulescu
 * Last modified: Feb 16, 2021
 **/

#ifndef BASE_TEXTURE_H
#define BASE_TEXTURE_H

#include <GL/gl3w.h>

#include <IL/il.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

using std::cout;
using std::endl;
using std::exception;
using std::FILE;
using std::max;
using std::runtime_error;
using std::string;



class BaseTexture {
public:
	virtual ~BaseTexture();

	virtual void startReading(GLint textureUnit) const;
	virtual void stopReading() const;

	GLsizei getWidth() const;
	GLsizei getHeight() const;

	GLuint getId() const;

protected:
	static const unsigned int knCubeFaces_;

	BaseTexture();

	void loadIlImage_(const string& kFilePath, const string& kFileName, bool cubeMap, GLuint face);
	void loadDDsImage_(const string& kFilePath, const string& kFileName, bool cubeMap, bool cubeDDS, GLuint face);

	virtual void initTexture_(bool mipmapping);

	virtual void setTexParameters_(bool repeat, bool linearFiltering, bool mipmapping, bool compareMode) const;
	virtual void generateMipmaps_(bool mipmapping);
	virtual void computeTexTransparency_();

	void setTexImage_(GLint level, GLsizei width, GLsizei height, GLsizei compressedSize, const GLubyte* pkTextureData) const;

	GLubyte** ppTextureData_;
	GLsizei* pOffsets_;
	string filePath_, fileName_;
	GLsizei width_, height_;
	GLint nMipmaps_, internalFormat_, maxCombinedTextureImageUnits_;
	GLenum format_, dataType_;	
	bool dds_, compressed_, transparent_;
	GLuint id_;

private:
	BaseTexture(const BaseTexture&);
	const BaseTexture& operator=(const BaseTexture&) {}

	void readDDsHeader_(FILE* pFile, const string& kFileName, long int& rFileLength, GLsizei& rHeight, GLsizei& rWidth,
		                GLint& rnMipmaps, GLuint*& rpPixelFormat, GLuint& rCaps2) const;

	void getTextureFormat_(const GLuint* pkPixelFormat,
		                   GLint& rInternalFormat, GLenum& rFormat, GLuint& rBlockSize, bool& rCompressed) const;
	void getNumFaces_(GLuint caps2, bool cubeDDS, GLuint& rnFaces) const;

	void setTextureParameters_(const string& kFilePath, const string& kFileName, bool cubeMap, bool cubeDDS, GLuint face,
	                           GLsizei width, GLsizei height, GLint nMipmaps, 
		                       GLint internalFormat, GLenum format, GLenum dataType, bool compressed);
};

#endif
