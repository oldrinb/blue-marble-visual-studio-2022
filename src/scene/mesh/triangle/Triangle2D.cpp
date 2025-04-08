/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 21, 2021
 **/

#include "Triangle2D.h"



Triangle2D::Triangle2D() : nVertices_(3), pVertices2d_(nullptr), pTexCoords_(nullptr), pColors_(nullptr),
	                       vaos_(), verticesVbo_(0u), texCoordsVbo_(0u), colorsVbo_(0u) {
	glGenBuffers(1, &verticesVbo_);
	glGenBuffers(1, &texCoordsVbo_);
	glGenBuffers(1, &colorsVbo_);

	//cout << "Triangle2D created." << endl;
}



Triangle2D::~Triangle2D() {	
	clearVerticesData_();
	
	glDeleteBuffers(1, &verticesVbo_);
	glDeleteBuffers(1, &texCoordsVbo_);
	glDeleteBuffers(1, &colorsVbo_);
	
	for (GLuint iVao : vaos_)
		if (iVao != 0u) glDeleteVertexArrays(1, &iVao);
	
	//cout << "Triangle2D deleted." << endl;
}



void Triangle2D::loadVertices(const vec2* pkVertices) {
	try {
		loadVertices_(pkVertices);
	}
	catch (const exception& kException) {
		throw runtime_error("Triangle2D.loadVertices > " + string(kException.what()));
	}
}



void Triangle2D::loadTexCoords(const vec2* pkTexCoords) {
	if (!pVertices2d_) throw runtime_error("Triangle2D.loadTexCoords|Vertices not loaded yet.");

	try {
		loadTexCoords_(pkTexCoords);
	}
	catch (const exception& kException) {
		throw runtime_error("Triangle2D.loadTexCoords > " + string(kException.what()));
	}
}



void Triangle2D::loadColors(const vec3* pkColors) {
	if (!pVertices2d_) throw runtime_error("Triangle2D.loadColors|Vertices not loaded yet.");

	try {
		loadColors_(pkColors);
	}
	catch (const exception& kException) {
		throw runtime_error("Triangle2D.loadColors > " + string(kException.what()));
	}
}



void Triangle2D::updateVertexBuffer() const {
	if (!pVertices2d_) throw runtime_error("Triangle2D.updateVertexBuffer|Vertices not loaded yet.");

	updateVertexBuffer_();
}



void Triangle2D::setAttribPointers(unsigned int programId, GLuint positionIndex) {
	setAttribPointers_(programId, positionIndex, 0u, 0u, 0u, 0u, 1u);
}



void Triangle2D::setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex) {
	setAttribPointers_(programId, positionIndex, texCoordIndex, 0u, 0u, 0u, 2u);
}



void Triangle2D::setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex, GLuint colorIndex) {
	setAttribPointers_(programId, positionIndex, texCoordIndex, colorIndex, 0u, 0u, 3u);
}



void Triangle2D::render(unsigned int programId) const {
	if (!pVertices2d_) throw runtime_error("Triangle2D.render|Vertices not loaded yet.");
	if (!glIsBuffer(verticesVbo_))
		throw runtime_error("Triangle2D.render|Vertex buffer data not loaded yet: vertices.");

	if (pTexCoords_ && !glIsBuffer(texCoordsVbo_))
		throw runtime_error("Triangle2D.render_|Vertex buffer data not loaded yet: tex coords.");

	if (pColors_ && !glIsBuffer(colorsVbo_))
		throw runtime_error("Triangle2D.render|Vertex buffer data not loaded yet: colors.");
	
	try {
		render_(programId);
	}
	catch (const exception& kException) {
		throw runtime_error("Triangle2D.render > " + string(kException.what()));
	}
}



unsigned int Triangle2D::getNumVertices() const {
	return (pVertices2d_) ? nVertices_ : 0u;
}



const vec2* Triangle2D::getVertex(unsigned int id) const {
	if (pVertices2d_ && static_cast<GLsizei>(id) < nVertices_)
		return &pVertices2d_[id];
	else throw runtime_error("Triangle2D.getVertex|Invalid id value or vertices not loaded yet.");
}



const vec2* Triangle2D::getTexCoord(unsigned int id) const {
	if (pTexCoords_ && static_cast<GLsizei>(id) < nVertices_)
		return &pTexCoords_[id];
	else throw runtime_error("Triangle2D.getTexCoord|Invalid id value or tex coords not loaded yet.");
}



const vec3* Triangle2D::getColor(unsigned int id) const {
	if (pColors_ && static_cast<GLsizei>(id) < nVertices_)
		return &pColors_[id];
	else throw runtime_error("Triangle2D.getColor|Invalid id value or colors not loaded yet.");
}



bool Triangle2D::hasVertices() const {
	return pVertices2d_ && (nVertices_ > 0);
}



bool Triangle2D::hasTexCoords() const {
	return pTexCoords_ && (nVertices_ > 0);
}



bool Triangle2D::hasColors() const {
	return pColors_ && (nVertices_ > 0);
}



void Triangle2D::clearVerticesData_() {
	if (pVertices2d_) delete[] pVertices2d_; pVertices2d_ = nullptr;
	if (pTexCoords_) delete[] pTexCoords_; pTexCoords_ = nullptr;
	if (pColors_) delete[] pColors_; pColors_ = nullptr;
}



void Triangle2D::loadVertices_(const vec2* pkVertices) {
	if (pVertices2d_) delete[] pVertices2d_;
	pVertices2d_ = new vec2[static_cast<size_t>(nVertices_)];

	try {
		std::copy(pkVertices, pkVertices + nVertices_, pVertices2d_);
	}
	catch (const exception& kException) {
		throw runtime_error("Triangle2D.loadVertices_ > copy > " + string(kException.what()));
	}
}



void Triangle2D::loadTexCoords_(const vec2* pkTexCoords) {
	if (pTexCoords_) delete[] pTexCoords_;
	pTexCoords_ = new vec2[static_cast<size_t>(nVertices_)];

	try {
		std::copy(pkTexCoords, pkTexCoords + nVertices_, pTexCoords_);
	}
	catch (const exception& kException) {
		throw runtime_error("Triangle2D.loadTexCoords_ > copy > " + string(kException.what()));
	}
}



void Triangle2D::loadColors_(const vec3* pkColors) {
	if (pColors_) delete[] pColors_;
	pColors_ = new vec3[static_cast<size_t>(nVertices_)];

	try {
		std::copy(pkColors, pkColors + nVertices_, pColors_);
	}
	catch (const exception& kException) {
		throw runtime_error("Triangle2D.loadColors_ > copy > " + string(kException.what()));
	}
}



void Triangle2D::updateVertexBuffer_() const {
	GLsizeiptr size2 = static_cast<GLsizeiptr>(2 * sizeof(GLfloat) * nVertices_);
	GLsizeiptr size3 = static_cast<GLsizeiptr>(3 * sizeof(GLfloat) * nVertices_);

	if (pVertices2d_) {
		GLfloat* pGlVertices = new GLfloat[static_cast<size_t>(nVertices_) * 2u];

		for (GLsizei i = 0; i < nVertices_; i++) {
			pGlVertices[2u * i] = pVertices2d_[i].x;
			pGlVertices[2u * i + 1u] = pVertices2d_[i].y;
		}

		glBindBuffer(GL_ARRAY_BUFFER, verticesVbo_);
		glBufferData(GL_ARRAY_BUFFER, size2, pGlVertices, GL_STATIC_DRAW);

		delete[] pGlVertices;
	}

	if (pTexCoords_) {
		GLfloat* pGlTexCoords = new GLfloat[static_cast<size_t>(nVertices_) * 2u];

		for (GLsizei i = 0; i < nVertices_; i++) {
			pGlTexCoords[2u * i] = pTexCoords_[i].s;
			pGlTexCoords[2u * i + 1u] = pTexCoords_[i].t;
		}

		glBindBuffer(GL_ARRAY_BUFFER, texCoordsVbo_);
		glBufferData(GL_ARRAY_BUFFER, size2, pGlTexCoords, GL_STATIC_DRAW);

		delete[] pGlTexCoords;
	}

	if (pColors_) {
		GLfloat* pGlColors = new GLfloat[static_cast<size_t>(nVertices_) * 3u];

		for (GLsizei i = 0; i < nVertices_; i++) {
			pGlColors[3u * i] = pColors_[i].x;
			pGlColors[3u * i + 1u] = pColors_[i].y;
			pGlColors[3u * i + 2u] = pColors_[i].z;
		}

		glBindBuffer(GL_ARRAY_BUFFER, colorsVbo_);
		glBufferData(GL_ARRAY_BUFFER, size3, pGlColors, GL_STATIC_DRAW);

		delete[] pGlColors;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0u);
}



void Triangle2D::setAttribPointers_(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex, GLuint normalIndex,
	                                GLuint, GLuint, unsigned int nIndices) {
	vaos_.resize(static_cast<size_t>(programId) + 1u, 0u);

	if (vaos_.at(programId) == 0u)
		glGenVertexArrays(1, &vaos_.at(programId));
	
	glBindVertexArray(vaos_.at(programId));

	if (pVertices2d_ && glIsBuffer(verticesVbo_) && nIndices > 0u) {
		glBindBuffer(GL_ARRAY_BUFFER, verticesVbo_);
		glVertexAttribPointer(positionIndex, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(positionIndex);
	}

	if (pTexCoords_ && glIsBuffer(texCoordsVbo_) && nIndices > 1u) {
		glBindBuffer(GL_ARRAY_BUFFER, texCoordsVbo_);
		glVertexAttribPointer(texCoordIndex, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(texCoordIndex);
	}

	if (pColors_ && glIsBuffer(colorsVbo_) && nIndices > 2u) {
		glBindBuffer(GL_ARRAY_BUFFER, colorsVbo_);
		glVertexAttribPointer(normalIndex, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(normalIndex);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0u);
	glBindVertexArray(0u);	
}



void Triangle2D::render_(unsigned int programId) const {
	if (programId < vaos_.size()) {
		GLuint vao = vaos_.at(programId);
		if (vao > 0u) {
			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, nVertices_);
			glBindVertexArray(0u);
		}
		else throw runtime_error("Triangle2D.render_|Invalid program id value.");
	}
	else throw runtime_error("Triangle2D.render_|Invalid program id value.");
}
