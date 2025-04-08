/**
 * Author: Oldrin Barbulescu
 * Last modified: Feb 16, 2021
 **/

#ifndef COLOR_TEXTURE_H
#define COLOR_TEXTURE_H

#include <GL/gl3w.h>

#include "BaseTexture.h"

#include <algorithm>
#include <cstdio>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

using std::cout;
using std::endl;
using std::exception;
using std::max;
using std::runtime_error;
using std::size_t;
using std::string;



class ColorTexture : public BaseTexture {
public:
	enum class TextureType { NONE = 0u, DIFFUSE = 2u, SPECULAR = 3u, EMISSIVE = 4u, NORMAL_MAP = 5u };

	ColorTexture(GLsizei width, GLsizei height, bool repeat, bool linearFiltering);
	ColorTexture(const string& kFilePath, const string& kFileName, bool repeat, bool linearFiltering, bool mipmapping);
	virtual ~ColorTexture();


	// render: 1) startReading
	//         2) stopReading
	//############################################################################
	//-> virtual void startReading(GLint textureUnit) const;
	//-> virtual void stopReading() const;
	

	// get
	//############################################################################
	//-> GLsizei getWidth() const;
	//-> GLsizei getHeight() const;

	string getFilePath() const;
	string getFileName() const;

	bool isDDS() const;
	bool isTransparent() const;

	//-> GLuint getId() const;	

private:
	ColorTexture(const ColorTexture&);
	const ColorTexture& operator=(const ColorTexture&) {}

	virtual void initTexture_(bool mipmapping);
};

#endif
