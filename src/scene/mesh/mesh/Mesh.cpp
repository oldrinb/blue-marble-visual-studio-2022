/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 31, 2025
 **/

#include "Mesh.h"



Mesh::Mesh(unsigned int id, unsigned int modelId, const Material* pkMaterial, 
	       unsigned int nFaces, unsigned int nVertices, bool transparencySorting) : Face(id, id, modelId, pkMaterial),
	       pTriangleStrip_(nullptr), pFaces_(nullptr), transparentFaces_(), transparencySorting_(false) {
	if (nVertices < 3u) 
		throw runtime_error("Mesh|Invalid number of vertices value.");
	else if (nVertices == 3u && nFaces != 1u) 
		throw runtime_error("Mesh|Invalid number of faces value.");
	else if (nVertices > 3u && nFaces < 2u) 
		throw runtime_error("Mesh|Invalid number of faces value.");

	nVertices_ = nVertices;
	nFaces_ = nFaces;

	delete pTriangle_; pTriangle_ = nullptr;
	pTriangleStrip_ = new TriangleStrip(nVertices, nFaces * 3u);

	if (pkMaterial->isTransparent()) transparencySorting_ = transparencySorting;

	//cout << "Mesh " << toString() << " created." << endl;
}



Mesh::~Mesh() {
	delete pTriangleStrip_;
	if (pFaces_) delete[] pFaces_;

	//cout << "Mesh " << toString() << " deleted." << endl;
}



void Mesh::loadFaces(const uvec3* pkFaces) {
	if (pFaces_) throw runtime_error("Mesh.loadFaces|Mesh faces already loaded.");
	pFaces_ = new uvec3[nFaces_];

	try {
		std::copy(pkFaces, pkFaces + nFaces_, pFaces_);
	}
	catch (const exception& kException) {
		throw runtime_error("Mesh.loadFaces > copy > " + string(kException.what()));
	}

	uvec1* pIndices = new uvec1[static_cast<size_t>(nFaces_) * 3u];
	
	for (unsigned int i = 0u; i < nFaces_; i++) {
		pIndices[3u * i].x = pkFaces[i].x;
		pIndices[3u * i + 1u].x = pkFaces[i].y;
		pIndices[3u * i + 2u].x = pkFaces[i].z;
	}

	try {
		pTriangleStrip_->loadIndices(pIndices);
	}
	catch (const exception& kException) {
		delete[] pIndices;
		throw runtime_error("Mesh.loadFaces > " + string(kException.what()));
	}

	delete[] pIndices;

	if (transparencySorting_) {
		transparentFaces_.clear();

		for (unsigned int i = 0u; i < nFaces_; i++) {
			try {
				transparentFaces_.emplace_back(i, kId_, kModelId_, pkMaterial_);
				Face* pFace = &transparentFaces_.back();

				pFace->setTwoSided(twoSided_);
				pFace->setWireframe(wireframe_);

				pFace->transform(transformationMatrix_);
				pFace->setRotationSpeed(rotationSpeed_);
			}
			catch (const exception& kException) {
				throw runtime_error("Mesh.loadFaces > " + string(kException.what()));
			}
		}
	}
}



void Mesh::loadVertices(const vec3* pkVertices) {
	if (!pFaces_) throw runtime_error("Mesh.loadVertices|Faces not loaded yet.");

	try {
		pTriangleStrip_->loadVertices(pkVertices);

		if (transparencySorting_)
			for (unsigned int i = 0u; i < nFaces_; i++) {
				vec3 vert[3u];
				vert[0u] = pkVertices[pFaces_[i].x];
				vert[1u] = pkVertices[pFaces_[i].y];
				vert[2u] = pkVertices[pFaces_[i].z];

				for (Face& iFace : transparentFaces_)
					if (iFace.getMeshId() == kId_ && iFace.getId() == i) {
						iFace.loadVertices(vert);
						break;
					}
			}		
	}
	catch (const exception& kException) {
		throw runtime_error("Mesh.loadVertices > " + string(kException.what()));
	}
}



void Mesh::loadTexCoords(const vec2* pkTexCoords) {
	if (!pFaces_) throw runtime_error("Mesh.loadTexCoords|Faces not loaded yet.");

	try {
		pTriangleStrip_->loadTexCoords(pkTexCoords);

		if (transparencySorting_)
			for (unsigned int i = 0u; i < nFaces_; i++) {
				vec2 tex[3u];
				tex[0u] = pkTexCoords[pFaces_[i].x];
				tex[1u] = pkTexCoords[pFaces_[i].y];
				tex[2u] = pkTexCoords[pFaces_[i].z];
				
				for (Face& iFace : transparentFaces_)
					if (iFace.getMeshId() == kId_ && iFace.getId() == i) {
						iFace.loadTexCoords(tex);
						break;
					}
			}		
	}
	catch (const exception& kException) {
		throw runtime_error("Mesh.loadTexCoords > " + string(kException.what()));
	}
}



void Mesh::loadNormals(const vec3* pkNormals) {
	if (!pFaces_) throw runtime_error("Mesh.loadNormals|Faces not loaded yet.");

	try {
		pTriangleStrip_->loadNormals(pkNormals);

		if (transparencySorting_)
			for (unsigned int i = 0u; i < nFaces_; i++) {
				vec3 norm[3u];
				norm[0u] = pkNormals[pFaces_[i].x];
				norm[1u] = pkNormals[pFaces_[i].y];
				norm[2u] = pkNormals[pFaces_[i].z];

				for (Face& iFace : transparentFaces_)
					if (iFace.getMeshId() == kId_ && iFace.getId() == i) {
						iFace.loadNormals(norm);
						break;
					}
			}		
	}
	catch (const exception& kException) {
		throw runtime_error("Mesh.loadNormals > " + string(kException.what()));
	}
}



void Mesh::loadTangentsAndBitangents(const vec3* pkTangents, const vec3* pkBitangents) {
	if (!pFaces_) throw runtime_error("Mesh.loadTangentsAndBitangents|Faces not loaded yet.");

	try {
		pTriangleStrip_->loadTangentsAndBitangents(pkTangents, pkBitangents);

		if (transparencySorting_)
			for (unsigned int i = 0u; i < nFaces_; i++) {
				vec3 tg[3u];
				vec3 bitg[3u];

				tg[0u] = pkTangents[pFaces_[i].x];
				tg[1u] = pkTangents[pFaces_[i].y];
				tg[2u] = pkTangents[pFaces_[i].z];
				bitg[0u] = pkBitangents[pFaces_[i].x];
				bitg[1u] = pkBitangents[pFaces_[i].y];
				bitg[2u] = pkBitangents[pFaces_[i].z];

				for (Face& iFace : transparentFaces_)
					if (iFace.getMeshId() == kId_ && iFace.getId() == i) {
						iFace.loadTangentsAndBitangents(tg, bitg);
						break;
					}
			}		
	}
	catch (const exception& kException) {
		throw runtime_error("Mesh.loadTangentsAndBitangents > " + string(kException.what()));
	}
}



void Mesh::updateVertexBuffer() const {
	try {
		if (transparencySorting_)
			for (const Face& ikFace : transparentFaces_)
				ikFace.updateVertexBuffer();		
		else pTriangleStrip_->updateVertexBuffer();
	}
	catch (const exception& kException) {
		throw runtime_error("Mesh.updateVertexBuffer > " + string(kException.what()));
	}
}



void Mesh::setAttribPointers(unsigned int programId, GLuint positionIndex) {
	if (transparencySorting_)
		for (Face& iFace : transparentFaces_)
			iFace.setAttribPointers(programId, positionIndex);

	else pTriangleStrip_->setAttribPointers(programId, positionIndex);
}



void Mesh::setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex) {
	if (transparencySorting_)
		for (Face& iFace : transparentFaces_)
			iFace.setAttribPointers(programId, positionIndex, texCoordIndex);

	else pTriangleStrip_->setAttribPointers(programId, positionIndex, texCoordIndex);
}



void Mesh::setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex, GLuint normalIndex) {
	if (transparencySorting_)
		for (Face& iFace : transparentFaces_)
			iFace.setAttribPointers(programId, positionIndex, texCoordIndex, normalIndex);

	else pTriangleStrip_->setAttribPointers(programId, positionIndex, texCoordIndex, normalIndex);
}



void Mesh::setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex, GLuint normalIndex,
	                         GLuint tangentIndex, GLuint bitangentIndex) {
	if (transparencySorting_)
		for (Face& iFace : transparentFaces_)
			iFace.setAttribPointers(programId, positionIndex, texCoordIndex, normalIndex, tangentIndex, bitangentIndex);
	
	else pTriangleStrip_->setAttribPointers(programId, positionIndex, texCoordIndex, normalIndex, tangentIndex, bitangentIndex);
}



void Mesh::setTwoSided(bool twoSided) {
	Face::setTwoSided(twoSided);
	for (Face& iFace : transparentFaces_)
		iFace.setTwoSided(twoSided);
}



void  Mesh::setWireframe(bool wireframe) {
	Face::setWireframe(wireframe);
	for (Face& iFace : transparentFaces_)
		iFace.setWireframe(wireframe);
}



void Mesh::transform(const mat4& kMatrix) {
	Face::transform(kMatrix);
	for (Face& iFace : transparentFaces_)
		try {
			iFace.transform(kMatrix);
		}
		catch (const exception& kException) {
			throw runtime_error("Mesh.transform > " + string(kException.what()));
		}
}



void Mesh::translate(const vec3& kDistance) {
	Face::translate(kDistance);

	for (Face& iFace : transparentFaces_)
		try {
			iFace.translate(kDistance);
		}
		catch (const exception& kException) {
			throw runtime_error("Mesh.translate > " + string(kException.what()));
		}
}



void Mesh::scale(const vec3& kScale) {
	Face::scale(kScale);
	for (Face& iFace : transparentFaces_)
		try {
			iFace.scale(kScale);
		}
		catch (const exception& kException) {
			throw runtime_error("Mesh.scale > " + string(kException.what()));
		}
}



void Mesh::rotate(float angle, const vec3& kAxis) { // degrees
	Face::rotate(angle, kAxis);
	for (Face& iFace : transparentFaces_)
		try {
			iFace.rotate(angle, kAxis);
		}
		catch (const exception& kException) {
			throw runtime_error("Mesh.rotate > " + string(kException.what()));
		}
}



void Mesh::setRotationSpeed(float speed) {
	Face::setRotationSpeed(speed);
	for (Face& iFace : transparentFaces_)
		iFace.setRotationSpeed(speed);
}



void Mesh::updateRotation(float deltaTime) {
	Face::updateRotation(deltaTime);
	for (Face& iFace : transparentFaces_)
		iFace.updateRotation(deltaTime);
}



void Mesh::render(unsigned int programId) const {
	try {
		if (transparencySorting_)
			for (const Face& ikFace : transparentFaces_)
				ikFace.render(programId);		
		else pTriangleStrip_->render(programId);
	}
	catch (const exception& kException) {
		throw runtime_error("Mesh.render > " + string(kException.what()));
	}
}



unsigned int Mesh::getNumFaces() const {
	return pFaces_ ? nFaces_ : 0u;
}



unsigned int Mesh::getNumVertices() const {
	return pTriangleStrip_->getNumVertices();
}



const vec3* Mesh::getVertex(unsigned int id) const {
	try {
		return pTriangleStrip_->getVertex(id);
	}
	catch (const exception& kException) {
		throw runtime_error("Mesh.getVertex > " + string(kException.what()));
	}
}



const vec2* Mesh::getTexCoord(unsigned int id) const {
	try {
		return pTriangleStrip_->getTexCoord(id);
	}
	catch (const exception& kException) {
		throw runtime_error("Mesh.getTexCoord > " + string(kException.what()));
	}
}



const vec3* Mesh::getNormal(unsigned int id) const {
	try {
		return pTriangleStrip_->getNormal(id);
	}
	catch (const exception& kException) {
		throw runtime_error("Mesh.getNormal > " + string(kException.what()));
	}
}



const vec3* Mesh::getTangent(unsigned int id) const {
	try {
		return pTriangleStrip_->getTangent(id);
	}
	catch (const exception& kException) {
		throw runtime_error("Mesh.getTangent > " + string(kException.what()));
	}
}



const vec3* Mesh::getBitangent(unsigned int id) const {
	try {
		return pTriangleStrip_->getBitangent(id);
	}
	catch (const exception& kException) {
		throw runtime_error("Mesh.getBitangent > " + string(kException.what()));
	}
}



bool Mesh::hasFaces() const {
	return (pFaces_ && (nFaces_ > 0u));
}



bool Mesh::hasVertices() const {
	return pTriangleStrip_->hasVertices();
}



bool Mesh::hasTexCoords() const {
	return pTriangleStrip_->hasTexCoords();
}



bool Mesh::hasNormals() const {
	return pTriangleStrip_->hasNormals();
}



bool Mesh::hasTangentsAndBitangents() const {
	return pTriangleStrip_->hasTangentsAndBitangents();
}



unsigned int Mesh::getNumTransparentFaces() const {
	return static_cast<unsigned int>(transparentFaces_.size());
}



const Face* Mesh::getTransparentFace(unsigned int id) const {
	unsigned int i = 0u;
	for (const Face& ikFace : transparentFaces_) {
		if (i == id) return &ikFace;
		i++;
	}
	return nullptr;
}



bool Mesh::hasTransparentFaces() const {
	return (transparentFaces_.size() > 0u);
}



string Mesh::toString() const {
	return to_string(kModelId_) + "_" + to_string(kId_);
}
