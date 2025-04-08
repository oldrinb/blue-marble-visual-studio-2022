/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 21, 2021
 **/

#include "TriangleStrip.h"



TriangleStrip::TriangleStrip(unsigned int nVertices, unsigned int nIndices) :
	                         Triangle(), nIndices_(0), pIndices_(nullptr), indicesVbo_(0u) {
	if (nVertices >= 3u) nVertices_ = static_cast<GLsizei>(nVertices);
	else throw runtime_error("TriangleStrip|Invalid number of vertices value.");

	if (nIndices > 0u && nIndices % 3u == 0u) nIndices_ = static_cast<GLsizei>(nIndices);
	else throw runtime_error("TriangleStrip|Invalid number of indices value.");

	glGenBuffers(1, &indicesVbo_);

	//cout << "TriangleStrip created." << endl;
}



TriangleStrip::~TriangleStrip() {
	clearVerticesData_();

	glDeleteBuffers(1, &indicesVbo_);
	
	//cout << "TriangleStrip deleted." << endl;
}



void TriangleStrip::setNumVertices(unsigned int nVertices) {
	if (nVertices >= 3u) {
		nVertices_ = static_cast<GLsizei>(nVertices);

		clearVerticesData_();
		Triangle::clearVerticesData_();
		Triangle2D::clearVerticesData_();
	}
	else throw runtime_error("TriangleStrip.setNumVertices|Invalid number of vertices value.");
}



void TriangleStrip::setNumIndices(unsigned int nIndices) {
	if (nIndices > 0u && nIndices % 3u == 0u) {
		nIndices_ = static_cast<GLsizei>(nIndices);

		clearVerticesData_();
		Triangle::clearVerticesData_();
		Triangle2D::clearVerticesData_();
	}
	else throw runtime_error("TriangleStrip.setNumIndices|Invalid number of indices value.");
}



void TriangleStrip::loadIndices(const uvec1* pkIndices) {
	try {
		loadIndices_(pkIndices);
	}
	catch (const exception& kException) {
		throw runtime_error("TriangleStrip.loadIndices > " + string(kException.what()));
	}
}



void TriangleStrip::loadTexCoords(const vec2* pkTexCoords) {
	if (!pIndices_) throw runtime_error("TriangleStrip.loadTexCoords|Indices not loaded yet.");

	Triangle::loadTexCoords(pkTexCoords);
}



void TriangleStrip::loadNormals(const vec3* pkNormals) {
	if (!pIndices_) throw runtime_error("TriangleStrip.loadNormals|Indices not loaded yet.");

	Triangle::loadNormals(pkNormals);
}



void TriangleStrip::updateVertexBuffer() const {
	if (!pIndices_) throw runtime_error("TriangleStrip.updateVertexBuffer|Indices not loaded yet.");

	Triangle::updateVertexBuffer();
}



void TriangleStrip::render(unsigned int programId) const {
	if (!pIndices_) throw runtime_error("TriangleStrip.render|Indices not loaded yet.");
	if (!glIsBuffer(indicesVbo_))
		throw runtime_error("TriangleStrip.render|Vertex buffer data not loaded yet: indices.");

	Triangle::render(programId);
}



unsigned int TriangleStrip::getNumIndices() const {
	return pIndices_ ? nIndices_ : 0u;
}



const uvec1* TriangleStrip::getIndex(unsigned int id) const {
	if (pIndices_ && static_cast<GLsizei>(id) < nIndices_)
		return &pIndices_[id];
	else throw runtime_error("TriangleStrip.getIndex|Invalid id value or indices not loaded yet.");
}



bool TriangleStrip::hasIndices() const {
	return (pIndices_ && (nIndices_ > 0));
}



void TriangleStrip::clearVerticesData_() {
	if (pIndices_) delete[] pIndices_; pIndices_ = nullptr;
}



void TriangleStrip::loadIndices_(const uvec1* pkIndices) {
	if (pIndices_) delete[] pIndices_;
	pIndices_ = new uvec1[static_cast<size_t>(nIndices_)];

	try {
		std::copy(pkIndices, pkIndices + nIndices_, pIndices_);
	}
	catch (const exception& kException) {
		throw runtime_error("TriangleStrip.loadIndices_ > copy > " + string(kException.what()));
	}
}



void TriangleStrip::updateVertexBuffer_() const {
	Triangle::updateVertexBuffer_();

	GLsizeiptr size1 = static_cast<GLsizeiptr>(sizeof(GLuint) * nIndices_);

	if (pIndices_) {
		GLuint* pGlIndices = new GLuint[static_cast<size_t>(nIndices_)];
		for (GLsizei i = 0; i < nIndices_; i++)
			pGlIndices[i] = pIndices_[i].x;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVbo_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size1, pGlIndices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);

		delete[] pGlIndices;
	}	
}



void TriangleStrip::setAttribPointers_(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex, GLuint normalIndex,
	                                                           GLuint tangentIndex, GLuint bitangentIndex, unsigned int nIndices) {
	Triangle::setAttribPointers_(programId, positionIndex, texCoordIndex, normalIndex, tangentIndex, bitangentIndex, nIndices);

	if (pIndices_ && glIsBuffer(indicesVbo_)) {
		glBindVertexArray(vaos_.at(programId));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesVbo_);
		glBindVertexArray(0u);
	}
}



void TriangleStrip::render_(unsigned int programId) const {
	if (programId < vaos_.size()) {
		GLuint vao = vaos_.at(programId);
		if (vao > 0u) {
			glBindVertexArray(vao);
			glDrawElements(GL_TRIANGLES, nIndices_, GL_UNSIGNED_INT, NULL);
			glBindVertexArray(0u);
		}
		else throw runtime_error("TriangleStrip.render_|Invalid program id value.");
	}
	else throw runtime_error("TriangleStrip.render_|Invalid program id value.");
}
