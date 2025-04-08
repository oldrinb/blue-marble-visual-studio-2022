/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 31, 2025
 **/

#ifndef MESH_H
#define MESH_H

#include <GL/gl3w.h>

#include "Face.h"
#include "material/Material.h"
#include "mesh/triangle/TriangleStrip.h"

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <glm/ext/vector_uint1.hpp>

#include <algorithm>
#include <exception>
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>

using glm::mat4;
using glm::uvec1;
using glm::uvec3;
using glm::vec2;
using glm::vec3;

using std::cout;
using std::endl;
using std::exception;
using std::list;
using std::runtime_error;
using std::string;
using std::to_string;



class Mesh : public Face {
public:
	Mesh(unsigned int id, unsigned int modelId, const Material* pkMaterial,
		 unsigned int nFaces, unsigned int nVertices, bool transparencySorting);
	virtual ~Mesh();


	// init: 1) loadFaces
	//       2) loadVertices
	//       3) [loadTexCoords], [loadNormals]
	//       4) [loadTangentsAndBitangents]
	//       5) updateVertexBuffer
	//       6) setAttribPointers (for each program)
	//############################################################################
	void loadFaces(const uvec3* pkFaces);

	virtual void loadVertices(const vec3* pkVertices);
	
	virtual void loadTexCoords(const vec2* pkTexCoords);
	virtual void loadNormals(const vec3* pkNormals);
	virtual void loadTangentsAndBitangents(const vec3* pkTangents, const vec3* pkBitangents);

	virtual void updateVertexBuffer() const;	

	virtual void setAttribPointers(unsigned int programId, GLuint positionIndex);
	virtual void setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex);
	virtual void setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex, GLuint normalIndex);
	virtual void setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex, GLuint normalIndex,
		                           GLuint tangentIndex, GLuint bitangentIndex);

	
	// set
	//############################################################################
	virtual void setTwoSided(bool twoSided);
	virtual void setWireframe(bool wireframe);

	virtual void transform(const mat4& kMatrix);
	virtual void translate(const vec3& kDistance);
	virtual void scale(const vec3& kScale);
	virtual void rotate(float angle, const vec3& kAxis); // degrees	

	virtual void setRotationSpeed(float speed);

	
	// render: 1) updateRotation
	//         2) render (with 'programId')
	//############################################################################
	virtual void updateRotation(float deltaTime);
	virtual void render(unsigned int programId) const;


	// get
	//############################################################################
	//-> const Material* getMaterial() const;

	unsigned int getNumFaces() const;

	virtual unsigned int getNumVertices() const;

	virtual const vec3* getVertex(unsigned int id) const; // id = 0 ... numVertices - 1
	virtual const vec2* getTexCoord(unsigned int id) const; // id = 0 ... numVertices - 1
	virtual const vec3* getNormal(unsigned int id) const; // id = 0 ... numVertices - 1
	virtual const vec3* getTangent(unsigned int id) const; // id = 0 ... numVertices - 1
	virtual const vec3* getBitangent(unsigned int id) const; // id = 0 ... numVertices - 1

	bool hasFaces() const;

	virtual bool hasVertices() const;
	virtual bool hasTexCoords() const;
	virtual bool hasNormals() const;
	virtual bool hasTangentsAndBitangents() const;

	unsigned int getNumTransparentFaces() const;
	const Face* getTransparentFace(unsigned int id) const; // id = 0 ... numTransparentFaces - 1
	bool hasTransparentFaces() const;	

	//-> bool isTwoSided() const;
	//-> bool isWireframe() const;
	//-> bool isTransparent() const;

	//-> const mat4* getModelMatrix() const;

	//-> unsigned int getId() const;
	//-> unsigned int getMeshId() const;
	//-> unsigned int getModelId() const;

	virtual string toString() const;

private:
	Mesh(const Mesh&);
	const Mesh& operator=(const Mesh&) {}

	TriangleStrip* pTriangleStrip_;
	uvec3* pFaces_;
	list<Face> transparentFaces_;
	bool transparencySorting_;
};

#endif
