/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 31, 2025
 **/

#ifndef FACE_H
#define FACE_H

#include <GL/gl3w.h>

#include "material/Material.h"
#include "mesh/triangle/Triangle.h"

#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/mat4x4.hpp>
#include <glm/matrix.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <glm/ext/matrix_transform.hpp>

#include <cmath>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;

using std::cout;
using std::endl;
using std::exception;
using std::runtime_error;
using std::string;
using std::to_string;



class Face {
public:
	Face(unsigned int id, unsigned int meshId, unsigned int modelId, const Material* pkMaterial);
	virtual ~Face();

	// init: 1) loadVertices
	//       2) [loadTexCoords], [loadNormals]
	//       3) [loadTangentsAndBitangents]
	//       4) updateVertexBuffer
	//       5) setAttribPointers (for each program)
	//############################################################################
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
	const Material* getMaterial() const;

	virtual unsigned int getNumVertices() const;

	virtual const vec3* getVertex(unsigned int id) const; // id = 0 ... numVertices - 1
	virtual const vec2* getTexCoord(unsigned int id) const; // id = 0 ... numVertices - 1
	virtual const vec3* getNormal(unsigned int id) const; // id = 0 ... numVertices - 1
	virtual const vec3* getTangent(unsigned int id) const; // id = 0 ... numVertices - 1
	virtual const vec3* getBitangent(unsigned int id) const; // id = 0 ... numVertices - 1

	virtual bool hasVertices() const;
	virtual bool hasTexCoords() const;
	virtual bool hasNormals() const;
	virtual bool hasTangentsAndBitangents() const;	

	bool isTwoSided() const;
	bool isWireframe() const;
	bool isTransparent() const;
	
	const mat4* getModelMatrix() const;

	unsigned int getId() const;
	unsigned int getMeshId() const;
	unsigned int getModelId() const;

	virtual string toString() const;

protected:
	const unsigned int kId_, kMeshId_, kModelId_;
	const Material* pkMaterial_;

	Triangle* pTriangle_;
	mat4 transformationMatrix_;
	unsigned int nVertices_, nFaces_;
	float rotationSpeed_;
	bool twoSided_, wireframe_;
	bool hasShadow_;

private:
	static const float kLengthEpsilon_;
	static const float kDetEpsilon_;

	static vec4 getYaxis_();

	Face(const Face&);
	const Face& operator=(const Face&) {}

	void updateModelMatrix_();

	mat4 modelMatrix_, rotationMatrix_;
	vec3 rotationAxis_;
	float rotationAngle_;		
};

#endif
