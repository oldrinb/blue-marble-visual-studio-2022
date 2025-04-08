/**
 * Author: Oldrin Barbulescu
 * Last modified: Feb 23, 2021
 **/

#ifndef TRIANGLE_LIST_2D_H
#define TRIANGLE_LIST_2D_H

#include <GL/gl3w.h>

#include "Triangle2D.h"

#include <iostream>
#include <stdexcept>

using std::cout;
using std::endl;
using std::runtime_error;



class TriangleList2D : public Triangle2D {
public:
	TriangleList2D(unsigned int nVertices);
	virtual ~TriangleList2D();


	// init: 1) [setNumVertices]
	//       2) loadVertices
	//       3) [loadTexCoords], [loadColors]
	//       4) updateVertexBuffer
	//       5) setAttribPointers (for each program)
	//############################################################################
	void setNumVertices(unsigned int nVertices);

	//-> void loadVertices(const vec2* pkVertices);

	//-> virtual void loadTexCoords(const vec2* pkTexCoords);
	//-> void loadColors(const vec3* pkColors);

	//-> virtual void updateVertexBuffer() const;

	//-> virtual void setAttribPointers(unsigned int programId, GLuint positionIndex);
	//-> virtual void setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex);
	//-> virtual void setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex, GLuint colorIndex);


	// render (with 'programId')
	//############################################################################
	//-> virtual void render(unsigned int programId) const;


	// get
	//############################################################################
	//-> virtual unsigned int getNumVertices() const;

	//-> const vec2* getVertex(unsigned int id) const;

	//-> const vec2* getTexCoord(unsigned int id) const;
	//-> const vec3* getColor(unsigned int id) const;

	//-> virtual bool hasVertices() const;

	//-> bool hasTexCoords() const;
	//-> bool hasColors() const;

private:
	TriangleList2D(const TriangleList2D&);
	const TriangleList2D& operator=(const TriangleList2D&) {}
};

#endif
