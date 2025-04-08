/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 13, 2021
 **/

#ifndef TRIANGLE_2D_H
#define TRIANGLE_2D_H

#include <GL/gl3w.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <algorithm>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using glm::vec2;
using glm::vec3;

using std::cout;
using std::endl;
using std::exception;
using std::runtime_error;
using std::string;
using std::vector;



class Triangle2D {
public:
	Triangle2D();
	virtual ~Triangle2D();
	

	// init: 1) loadVertices
	//       2) [loadTexCoords], [loadColors]
	//       3) updateVertexBuffer
	//       4) setAttribPointers (for each program)
	//############################################################################
	void loadVertices(const vec2* pkVertices);
	
	virtual void loadTexCoords(const vec2* pkTexCoords);
	void loadColors(const vec3* pkColors);
	
	virtual void updateVertexBuffer() const;
	
	virtual void setAttribPointers(unsigned int programId, GLuint positionIndex);
	virtual void setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex);
	virtual void setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex, GLuint colorIndex);
	

	// render (with 'programId')
	//############################################################################
	virtual void render(unsigned int programId) const;
	

	// get
	//############################################################################
	virtual unsigned int getNumVertices() const;
	
	const vec2* getVertex(unsigned int id) const;
	
	const vec2* getTexCoord(unsigned int id) const;
	const vec3* getColor(unsigned int id) const;
	
	virtual bool hasVertices() const;
	
	bool hasTexCoords() const;
	bool hasColors() const;
	
protected:
	virtual void clearVerticesData_();

	void loadVertices_(const vec2* pkVertices);
	void loadTexCoords_(const vec2* pkTexCoords);
	void loadColors_(const vec3* pkColors);
								
	virtual void updateVertexBuffer_() const;
	virtual void setAttribPointers_(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex, GLuint normalIndex,
		                                                    GLuint tangentIndex, GLuint bitangentIndex, unsigned int nIndices);

	virtual void render_(unsigned int programId) const;

	GLsizei nVertices_;
	vec2* pVertices2d_, * pTexCoords_;
	vec3* pColors_;

	vector<GLuint> vaos_;
	GLuint verticesVbo_, texCoordsVbo_, colorsVbo_;

private:
	Triangle2D(const Triangle2D&);
	const Triangle2D& operator=(const Triangle2D&) {}
};

#endif
