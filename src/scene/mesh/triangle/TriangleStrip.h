/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 13, 2021
 **/

#ifndef TRIANGLE_STRIP_H
#define TRIANGLE_STRIP_H

#include <GL/gl3w.h>

#include "Triangle.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <glm/ext/vector_uint1.hpp>

#include <algorithm>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

using glm::uvec1;
using glm::vec2;
using glm::vec3;

using std::cout;
using std::endl;
using std::exception;
using std::runtime_error;
using std::string;



class TriangleStrip : public Triangle {
public:
	TriangleStrip(unsigned int nVertices, unsigned int nIndices);
	virtual ~TriangleStrip();


	// init: 1) [setNumVertices], [setNumIndices]
	//       2) loadVertices, loadIndices
	//       3) [loadTexCoords], [loadNormals]
	//       4) [loadTangentsAndBitangents]
	//       5) updateVertexBuffer
	//       6) setAttribPointers (for each program)
	//############################################################################
	void setNumVertices(unsigned int nVertices);
	void setNumIndices(unsigned int nIndices);

	//-> void loadVertices(const vec3* pkVertices);
	void loadIndices(const uvec1* pkIndices);

	virtual void loadTexCoords(const vec2* pkTexCoords);
	virtual void loadNormals(const vec3* pkNormals);
	//-> void loadTangentsAndBitangents(const vec3* pkTangents, const vec3* pkBitangents);

	virtual void updateVertexBuffer() const;

	//-> virtual void setAttribPointers(unsigned int programId, GLuint positionIndex);
	//-> virtual void setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex);
	//-> virtual void setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex, GLuint normalIndex);
	//-> void setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex, GLuint normalIndex,
	//->                                                GLuint tangentIndex, GLuint bitangentIndex);


	// render (with 'programId')
	//############################################################################
	virtual void render(unsigned int programId) const;


	// get
	//############################################################################
	//-> virtual unsigned int getNumVertices() const;
	unsigned int getNumIndices() const;
	
	//-> const vec3* getVertex(unsigned int id) const;
	const uvec1* getIndex(unsigned int id) const;

	//-> const vec2* getTexCoord(unsigned int id) const;
	//-> const vec3* getNormal(unsigned int id) const;
	//-> const vec3* getTangent(unsigned int id) const;
	//-> const vec3* getBitangent(unsigned int id) const;
	
	//-> virtual bool hasVertices() const;
	bool hasIndices() const;

	//-> bool hasTexCoords() const;
	//-> bool hasNormals() const;
	//-> bool hasTangentsAndBitangents() const;

private:
	TriangleStrip(const TriangleStrip&);
	const TriangleStrip& operator=(const TriangleStrip&) {}

	virtual void clearVerticesData_();

	void loadIndices_(const uvec1* pkIndices);

	virtual void updateVertexBuffer_() const;
	virtual void setAttribPointers_(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex, GLuint normalIndex,
		                                                    GLuint tangentIndex, GLuint bitangentIndex, unsigned int nIndices);

	virtual void render_(unsigned int programId) const;

	GLsizei nIndices_;
	uvec1* pIndices_;
	GLuint indicesVbo_;
};

#endif
