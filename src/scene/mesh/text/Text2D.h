/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 15, 2021
 **/

#ifndef TEXT_2D_H
#define TEXT_2D_H

#include <GL/gl3w.h>

#include "mesh/triangle/TriangleList2D.h"

#include <glm/common.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <algorithm>
#include <cmath>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

using glm::uvec2;
using glm::vec2;
using glm::vec3;
using glm::vec4;

using std::cout;
using std::endl;
using std::exception;
using std::runtime_error;
using std::string;



class Text2D {
public:
	Text2D(const uvec2& kWindowSize);
	~Text2D();
	
	
	// init: 1) addText (for each token of text)
	//       2) updateVertexBuffer
	//       3) setAttribPointers
	//############################################################################
	// row, col = 0,1,...-2,-1; offset = -0.5f ... 0.5f (left, right, up, down)
	void addText(const string& kText, const vec2& kPosition, const vec3& kColor, bool bold = false);
	void addText(const string& kText, const vec2& kPosition, const vec3& kColor, const vec4& kOffset, bool bold = false);

	void updateVertexBuffer();
	void setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex, GLuint colorIndex);


	// set
	//############################################################################
	void setWindowSize(const uvec2& kSize);


	// render
	//############################################################################
	void render(unsigned int programId) const;
	
private:
	static unsigned int getMaxTextLength_();
	static unsigned int getTextWidth_();
	static unsigned int getTextHeight_();	
	static unsigned int getTextureSize_();

	Text2D(const Text2D&);
	const Text2D& operator=(const Text2D&) {}

	void clearText_();

	void addPosition_(const vec2& kPosition, unsigned int nVertices);
	void addVertices_(const vec2& kTextCoords, unsigned int nVertices, const vec4& kOffset);
	void addTextureCoords_(const string& kText, unsigned int nVertices, const vec4& kOffset, bool bold);
	void addColors_(const vec3& kColor, unsigned int nVertices);
	
	vec2 getTextCoordinates_(float row, float col) const;	

	unsigned int nTotalVertices_;
	TriangleList2D* pTriangleList2D_;
	uvec2 windowSize_;

	vec2* pPosition_, * pVertices_, * pTextureCoords_;
	vec3* pColors_;
	float nbCharU_, nbCharV_;
};

#endif
