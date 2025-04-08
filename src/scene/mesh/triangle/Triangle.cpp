/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 21, 2021
 **/

#include "Triangle.h"



Triangle::Triangle() : Triangle2D(), pVertices_(nullptr), pNormals_(nullptr), pTangents_(nullptr), pBitangents_(nullptr),
                       normalsVbo_(0u), tangentsVbo_(0u), bitangentsVbo_(0u) {
	glGenBuffers(1, &normalsVbo_);
	glGenBuffers(1, &tangentsVbo_);
	glGenBuffers(1, &bitangentsVbo_);
	
	//cout << "Triangle created." << endl;
}



Triangle::~Triangle() {	
	clearVerticesData_();

	glDeleteBuffers(1, &normalsVbo_);
	glDeleteBuffers(1, &tangentsVbo_);
	glDeleteBuffers(1, &bitangentsVbo_);

	//cout << "Triangle deleted." << endl;
}



void Triangle::loadVertices(const vec3* pkVertices) {
	try {
		loadVertices_(pkVertices);
	}
	catch (const exception& kException) {
		throw runtime_error("Triangle.loadVertices > " + string(kException.what()));
	}
}



void Triangle::loadTexCoords(const vec2* pkTexCoords) {
	if (!pVertices_) throw runtime_error("Triangle.loadTexCoords|Vertices not loaded yet.");

	try {
		loadTexCoords_(pkTexCoords);
	}
	catch (const exception& kException) {
		throw runtime_error("Triangle.loadTexCoords > " + string(kException.what()));
	}
}



void Triangle::loadNormals(const vec3* pkNormals) {
	if (!pVertices_) throw runtime_error("Triangle.loadNormals|Vertices not loaded yet.");

	try {
		loadNormals_(pkNormals);
	}
	catch (const exception& kException) {
		throw runtime_error("Triangle.loadNormals > " + string(kException.what()));
	}
}



void Triangle::loadTangentsAndBitangents(const vec3* pkTangents, const vec3* pkBitangents) {
	if (!pNormals_) throw runtime_error("Triangle.loadTangentsAndBitangents|Normals not loaded yet.");

	try {
		loadTangents_(pkTangents);
		loadBitangents_(pkBitangents);
	}
	catch (const exception& kException) {
		throw runtime_error("Triangle.loadTangentsAndBitangents > " + string(kException.what()));
	}
}



void Triangle::updateVertexBuffer() const {
	if (!pVertices_) throw runtime_error("Triangle.updateVertexBuffer|Vertices not loaded yet.");
	if (!pNormals_) throw runtime_error("Triangle.updateVertexBuffer|Normals not loaded yet.");

	updateVertexBuffer_();
}



void Triangle::setAttribPointers(unsigned int programId, GLuint positionIndex) {
	setAttribPointers_(programId, positionIndex, 0u, 0u, 0u, 0u, 1u);
}



void Triangle::setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex) {
	setAttribPointers_(programId, positionIndex, texCoordIndex, 0u, 0u, 0u, 2u);
}



void Triangle::setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex, GLuint normalIndex) {
	setAttribPointers_(programId, positionIndex, texCoordIndex, normalIndex, 0u, 0u, 3u);
}



void Triangle::setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex, GLuint normalIndex,
	                                                     GLuint tangentIndex, GLuint bitangentIndex) {
	setAttribPointers_(programId, positionIndex, texCoordIndex, normalIndex, tangentIndex, bitangentIndex, 5u);
}



void Triangle::render(unsigned int programId) const {
	if (!pVertices_) throw runtime_error("Triangle.render|Vertices not loaded yet.");
	if (!glIsBuffer(verticesVbo_))
		throw runtime_error("Triangle.render|Vertex buffer data not loaded yet: vertices.");

	if (pTexCoords_ && !glIsBuffer(texCoordsVbo_))
		throw runtime_error("Triangle.render_|Vertex buffer data not loaded yet: tex coords.");

	if (!pNormals_) throw runtime_error("Triangle.render|Normals not loaded yet.");	
	if (!glIsBuffer(normalsVbo_))
		throw runtime_error("Triangle.render|Vertex buffer data not loaded yet: normals.");

	if (pTangents_ && !glIsBuffer(tangentsVbo_))
		throw runtime_error("Triangle.render|Vertex buffer data not loaded yet: tangents.");
	if (pBitangents_ && !glIsBuffer(bitangentsVbo_))
		throw runtime_error("Triangle.render|Vertex buffer data not loaded yet: bitangents.");

	try {
		render_(programId);
	}
	catch (const exception& kException) {
		throw runtime_error("Triangle.render > " + string(kException.what()));
	}
}



unsigned int Triangle::getNumVertices() const {
	return (pVertices_) ? nVertices_ : 0u;
}



const vec3* Triangle::getVertex(unsigned int id) const {
	if (pVertices_ && static_cast<GLsizei>(id) < nVertices_)
		return &pVertices_[id];
	else throw runtime_error("Triangle.getVertex|Invalid id value or vertices not loaded yet.");
}



const vec3* Triangle::getNormal(unsigned int id) const {
	if (pNormals_ && static_cast<GLsizei>(id) < nVertices_)
		return &pNormals_[id];
	else throw runtime_error("Triangle.getNormal|Invalid id value or normals not loaded yet.");
}



const vec3* Triangle::getTangent(unsigned int id) const {
	if (pTangents_ && static_cast<GLsizei>(id) < nVertices_)
		return &pTangents_[id];
	else throw runtime_error("Triangle.getTangent|Invalid id value or tangents not loaded yet.");
}



const vec3* Triangle::getBitangent(unsigned int id) const {
	if (pBitangents_ && static_cast<GLsizei>(id) < nVertices_)
		return &pBitangents_[id];
	else throw runtime_error("Triangle.getBitangent|Invalid id value or bitangents not loaded yet.");
}



bool Triangle::hasVertices() const {
	return pVertices_ && (nVertices_ > 0);
}



bool Triangle::hasNormals() const {
	return pNormals_ && (nVertices_ > 0);
}



bool Triangle::hasTangentsAndBitangents() const {
	return pTangents_ && pBitangents_ && (nVertices_ > 0);
}



void Triangle::clearVerticesData_() {
	if (pVertices_) delete[] pVertices_; pVertices_ = nullptr;
	if (pNormals_) delete[] pNormals_; pNormals_ = nullptr;
	if (pTangents_) delete[] pTangents_; pTangents_ = nullptr;
	if (pBitangents_) delete[] pBitangents_; pBitangents_ = nullptr;
}



void Triangle::loadVertices_(const vec3* pkVertices) {
	if (pVertices_) delete[] pVertices_;
	pVertices_ = new vec3[static_cast<size_t>(nVertices_)];

	try {
		std::copy(pkVertices, pkVertices + nVertices_, pVertices_);
	}
	catch (const exception& kException) {
		throw runtime_error("Triangle.loadVertices_ > copy > " + string(kException.what()));
	}
}



void Triangle::loadNormals_(const vec3* pkNormals) {
	if (pNormals_) delete[] pNormals_;
	pNormals_ = new vec3[static_cast<size_t>(nVertices_)];

	try {
		std::copy(pkNormals, pkNormals + nVertices_, pNormals_);
	}
	catch (const exception& kException) {
		throw runtime_error("Triangle.loadNormals_ > copy > " + string(kException.what()));
	}
}



void Triangle::loadTangents_(const vec3* pkTangents) {
	if (pTangents_) delete[] pTangents_;
	pTangents_ = new vec3[static_cast<size_t>(nVertices_)];

	try {
		std::copy(pkTangents, pkTangents + nVertices_, pTangents_);
	}
	catch (const exception& kException) {
		throw runtime_error("Triangle.loadTangents_ > copy > " + string(kException.what()));
	}
}



void Triangle::loadBitangents_(const vec3* pkBitangents) {
	if (pBitangents_) delete[] pBitangents_;
	pBitangents_ = new vec3[static_cast<size_t>(nVertices_)];

	try {
		std::copy(pkBitangents, pkBitangents + nVertices_, pBitangents_);
	}
	catch (const exception& kException) {
		throw runtime_error("Triangle.loadBitangents_ > copy > " + string(kException.what()));
	}
}



void Triangle::updateVertexBuffer_() const {
	Triangle2D::updateVertexBuffer_();

	GLsizeiptr size3 = static_cast<GLsizeiptr>(3 * sizeof(GLfloat) * nVertices_);

	if (pVertices_) {
		GLfloat* pGlVertices = new GLfloat[static_cast<size_t>(nVertices_) * 3u];

		for (GLsizei i = 0; i < nVertices_; i++) {
			pGlVertices[3u * i] = pVertices_[i].x;
			pGlVertices[3u * i + 1u] = pVertices_[i].y;
			pGlVertices[3u * i + 2u] = pVertices_[i].z;
		}

		glBindBuffer(GL_ARRAY_BUFFER, verticesVbo_);
		glBufferData(GL_ARRAY_BUFFER, size3, pGlVertices, GL_STATIC_DRAW);

		delete[] pGlVertices;
	}

	if (pNormals_) {
		GLfloat* pGlNormals = new GLfloat[static_cast<size_t>(nVertices_) * 3u];

		for (GLsizei i = 0; i < nVertices_; i++) {
			pGlNormals[3u * i] = pNormals_[i].x;
			pGlNormals[3u * i + 1u] = pNormals_[i].y;
			pGlNormals[3u * i + 2u] = pNormals_[i].z;
		}

		glBindBuffer(GL_ARRAY_BUFFER, normalsVbo_);
		glBufferData(GL_ARRAY_BUFFER, size3, pGlNormals, GL_STATIC_DRAW);

		delete[] pGlNormals;
	}

	if (pTangents_ && pBitangents_) {
		GLfloat* pGlTangents = new GLfloat[static_cast<size_t>(nVertices_) * 3u];
		GLfloat* pGlBitangents = new GLfloat[static_cast<size_t>(nVertices_) * 3u];

		for (GLsizei i = 0; i < nVertices_; i++) {
			pGlTangents[3u * i] = pTangents_[i].x;
			pGlTangents[3u * i + 1u] = pTangents_[i].y;
			pGlTangents[3u * i + 2u] = pTangents_[i].z;

			pGlBitangents[3u * i] = pBitangents_[i].x;
			pGlBitangents[3u * i + 1u] = pBitangents_[i].y;
			pGlBitangents[3u * i + 2u] = pBitangents_[i].z;
		}

		glBindBuffer(GL_ARRAY_BUFFER, tangentsVbo_);
		glBufferData(GL_ARRAY_BUFFER, size3, pGlTangents, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, bitangentsVbo_);
		glBufferData(GL_ARRAY_BUFFER, size3, pGlBitangents, GL_STATIC_DRAW);

		delete[] pGlTangents;
		delete[] pGlBitangents;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0u);
}



void Triangle::setAttribPointers_(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex, GLuint normalIndex,
	                                                      GLuint tangentIndex, GLuint bitangentIndex, unsigned int nIndices) {
	Triangle2D::setAttribPointers_(programId, positionIndex, texCoordIndex, normalIndex, tangentIndex, bitangentIndex, nIndices);

	glBindVertexArray(vaos_.at(programId));

	if (pVertices_ && glIsBuffer(verticesVbo_) && nIndices > 0u) {
		glBindBuffer(GL_ARRAY_BUFFER, verticesVbo_);
		glVertexAttribPointer(positionIndex, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(positionIndex);
	}

	if (pNormals_ && glIsBuffer(normalsVbo_) && nIndices > 2u) {
		glBindBuffer(GL_ARRAY_BUFFER, normalsVbo_);
		glVertexAttribPointer(normalIndex, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(normalIndex);
	}

	if (pTangents_ && pBitangents_ && glIsBuffer(tangentsVbo_) && glIsBuffer(bitangentsVbo_) && nIndices == 5u) {
		glBindBuffer(GL_ARRAY_BUFFER, tangentsVbo_);
		glVertexAttribPointer(tangentIndex, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(tangentIndex);

		glBindBuffer(GL_ARRAY_BUFFER, bitangentsVbo_);
		glVertexAttribPointer(bitangentIndex, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(bitangentIndex);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0u);
	glBindVertexArray(0u);
}
