/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 13, 2021
 **/

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <GL/gl3w.h>

#include "Triangle2D.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <algorithm>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

using glm::vec2;
using glm::vec3;

using std::cout;
using std::endl;
using std::exception;
using std::runtime_error;
using std::string;



class Triangle : public Triangle2D {
public:
	Triangle();
	virtual ~Triangle();


	// init: 1) loadVertices
	//       2) [loadTexCoords], [loadNormals]
	//       3) [loadTangentsAndBitangents]
	//       4) updateVertexBuffer
	//       5) setAttribPointers (for each program)
	//############################################################################
	void loadVertices(const vec3* pkVertices);

	virtual void loadTexCoords(const vec2* pkTexCoords);
	virtual void loadNormals(const vec3* pkNormals);
	void loadTangentsAndBitangents(const vec3* pkTangents, const vec3* pkBitangents);

	virtual void updateVertexBuffer() const;

	virtual void setAttribPointers(unsigned int programId, GLuint positionIndex);
	virtual void setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex);
	virtual void setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex, GLuint normalIndex);
	void setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex, GLuint normalIndex,
		                                           GLuint tangentIndex, GLuint bitangentIndex);


	// render (with 'programId')
	//############################################################################
	virtual void render(unsigned int programId) const;

	
	// get
	//############################################################################
	virtual unsigned int getNumVertices() const;

	const vec3* getVertex(unsigned int id) const;

	//-> const vec2* getTexCoord(unsigned int id) const;
	const vec3* getNormal(unsigned int id) const;
	const vec3* getTangent(unsigned int id) const;
	const vec3* getBitangent(unsigned int id) const;

	virtual bool hasVertices() const;

	//-> bool hasTexCoords() const;
	bool hasNormals() const;
	bool hasTangentsAndBitangents() const;

protected:
	virtual void clearVerticesData_();

	void loadVertices_(const vec3* pkVertices);
	void loadNormals_(const vec3* pkNormals);
	void loadTangents_(const vec3* pkTangents);
	void loadBitangents_(const vec3* pkBitangents);

	virtual void updateVertexBuffer_() const;
	virtual void setAttribPointers_(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex, GLuint normalIndex,
		                                                    GLuint tangentIndex, GLuint bitangentIndex, unsigned int nIndices);

private:
	Triangle(const Triangle&);
	const Triangle& operator=(const Triangle&) {}

	vec3* pVertices_, * pNormals_, * pTangents_, * pBitangents_;
	GLuint normalsVbo_, tangentsVbo_, bitangentsVbo_;
};

#endif
