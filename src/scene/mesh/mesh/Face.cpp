/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 31, 2025
 **/

#include "Face.h"



const float Face::kLengthEpsilon_ = 0.01f;
const float Face::kDetEpsilon_ = 0.000001f;



vec4 Face::getYaxis_() {
	return vec4(0.0f, 1.0f, 0.0f, 0.0f);
}



Face::Face(unsigned int id, unsigned int meshId, unsigned int modelId, const Material* pkMaterial):
	       kId_(id), kMeshId_(meshId), kModelId_(modelId), pkMaterial_(nullptr),
	       pTriangle_(nullptr), transformationMatrix_(mat4(1.0f)), nVertices_(3u), nFaces_(1u),
	       rotationSpeed_(0.0f), twoSided_(false), wireframe_(false), hasShadow_(false),
	       modelMatrix_(mat4(1.0f)), rotationMatrix_(mat4(1.0f)), rotationAxis_(vec3(0.0f, 1.0f, 0.0f)), rotationAngle_(0.0f) {

	if (pkMaterial) pkMaterial_ = pkMaterial;
	pTriangle_ = new Triangle();

	//cout << "Face " << toString() << " created." << endl;
}



Face::~Face() {
	if (pTriangle_) delete pTriangle_;

	//cout << "Face " << toString() << " deleted." << endl;
}



void Face::loadVertices(const vec3* pkVertices) {
	try {
		pTriangle_->loadVertices(pkVertices);
	}
	catch (const exception& kException) {
		throw runtime_error("Face.loadVertices > " + string(kException.what()));
	}
}



void Face::loadTexCoords(const vec2* pkTexCoords) {
	try {
		pTriangle_->loadTexCoords(pkTexCoords);
	}
	catch (const exception& kException) {
		throw runtime_error("Face.loadTexCoords > " + string(kException.what()));
	}
}



void Face::loadNormals(const vec3* pkNormals) {
	try {
		pTriangle_->loadNormals(pkNormals);
	}
	catch (const exception& kException) {
		throw runtime_error("Face.loadNormals > " + string(kException.what()));
	}
}



void Face::loadTangentsAndBitangents(const vec3* pkTangents, const vec3* pkBitangents) {
	try {
		pTriangle_->loadTangentsAndBitangents(pkTangents, pkBitangents);
	}
	catch (const exception& kException) {
		throw runtime_error("Face.loadTangentsAndBitangents > " + string(kException.what()));
	}
}



void Face::updateVertexBuffer() const {
	try { 
		pTriangle_->updateVertexBuffer();
	}
	catch (const exception& kException) {
		throw runtime_error("Face.updateVertexBuffer > " + string(kException.what()));
	}
}



void Face::setAttribPointers(unsigned int programId, GLuint positionIndex) {
	pTriangle_->setAttribPointers(programId, positionIndex);
}



void Face::setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex) {
	pTriangle_->setAttribPointers(programId, positionIndex, texCoordIndex);
}



void Face::setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex, GLuint normalIndex) {
	pTriangle_->setAttribPointers(programId, positionIndex, texCoordIndex, normalIndex);
}



void Face::setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex, GLuint normalIndex,
	                         GLuint tangentIndex, GLuint bitangentIndex) {
	pTriangle_->setAttribPointers(programId, positionIndex, texCoordIndex, normalIndex, tangentIndex, bitangentIndex);
}



void Face::setTwoSided(bool twoSided) {
	twoSided_ = twoSided;
}



void  Face::setWireframe(bool wireframe) {
	wireframe_ = wireframe;
}



void Face::transform(const mat4& kMatrix) {
	transformationMatrix_ = kMatrix * transformationMatrix_;
	float det = glm::determinant(transformationMatrix_);

	if (!glm::isnan(det) && glm::abs(det) > Face::kDetEpsilon_) {
		vec3 axis = vec3(glm::inverse(transformationMatrix_) * Face::getYaxis_());

		if (glm::length(axis) > Face::kLengthEpsilon_)
			rotationAxis_ = glm::normalize(axis);
		else throw runtime_error("Face.transform|Invalid transformation matrix.");
	}
	else throw runtime_error("Face.transform|Invalid transformation matrix.");

	updateModelMatrix_();
}



void Face::translate(const vec3& kDistance) {
	mat4 tr = glm::translate(mat4(1.0f), kDistance);
	transformationMatrix_ = tr * transformationMatrix_;
	float det = glm::determinant(transformationMatrix_);

	if (!glm::isnan(det) && glm::abs(det) > Face::kDetEpsilon_) {
		vec3 axis = vec3(glm::inverse(transformationMatrix_) * Face::getYaxis_());

		if (glm::length(axis) > Face::kLengthEpsilon_)
			rotationAxis_ = glm::normalize(axis);
		else throw runtime_error("Face.translatem|Invalid transformation matrix.");
	}
	else throw runtime_error("Face.translate|Invalid transformation matrix.");

	updateModelMatrix_();
}



void Face::scale(const vec3& kScale) {
	mat4 sc = glm::scale(mat4(1.0f), kScale);
	transformationMatrix_ = sc * transformationMatrix_;
	float det = glm::determinant(transformationMatrix_);

	if (!glm::isnan(det) && glm::abs(det) > Face::kDetEpsilon_) {
		vec3 axis = vec3(glm::inverse(transformationMatrix_) * Face::getYaxis_());

		if (glm::length(axis) > Face::kLengthEpsilon_)
			rotationAxis_ = glm::normalize(axis);
		else throw runtime_error("Face.scale|Invalid transformation matrix.");
	}
	else throw runtime_error("Face.scale|Invalid transformation matrix.");

	updateModelMatrix_();
}



void Face::rotate(float angle, const vec3& kAxis) {
	mat4 rot = glm::rotate(mat4(1.0f), glm::radians(angle), kAxis);
	transformationMatrix_ = rot * transformationMatrix_;
	float det = glm::determinant(transformationMatrix_);

	if (!glm::isnan(det) && glm::abs(det) > Face::kDetEpsilon_) {
		vec3 axis = vec3(glm::inverse(transformationMatrix_) * Face::getYaxis_());

		if (glm::length(axis) > Face::kLengthEpsilon_)
			rotationAxis_ = glm::normalize(axis);
		else throw runtime_error("Face.rotate|Invalid transformation matrix.");
	}
	else throw runtime_error("Face.rotate|Invalid transformation matrix.");

	updateModelMatrix_();
}



void Face::setRotationSpeed(float speed) {
	rotationSpeed_ = speed;
}



void Face::updateRotation(float deltaTime) {
	rotationAngle_ += deltaTime * rotationSpeed_;
	rotationAngle_ = 180.0f + std::remainder(rotationAngle_ - 180.0f, 360.0f);
	rotationMatrix_ = glm::rotate(mat4(1.0f), glm::radians(rotationAngle_), rotationAxis_);
	updateModelMatrix_();
}



void Face::render(unsigned int programId) const {
	try {
		pTriangle_->render(programId);
	}
	catch (const exception& kException) {
		throw runtime_error("Face.render > " + string(kException.what()));
	}
}



const Material* Face::getMaterial() const {
	return pkMaterial_;
}



unsigned int Face::getNumVertices() const {
	return pTriangle_->getNumVertices();
}



const vec3* Face::getVertex(unsigned int id) const {
	try {
		return pTriangle_->getVertex(id);
	}
	catch (const exception& kException) {
		throw runtime_error("Face.getVertex > " + string(kException.what()));
	}
}



const vec2* Face::getTexCoord(unsigned int id) const {
	try {
		return pTriangle_->getTexCoord(id);
	}
	catch (const exception& kException) {
		throw runtime_error("Face.getTexCoord > " + string(kException.what()));
	}
}



const vec3* Face::getNormal(unsigned int id) const {
	try {
		return pTriangle_->getNormal(id);
	}
	catch (const exception& kException) {
		throw runtime_error("Face.getNormal > " + string(kException.what()));
	}
}



const vec3* Face::getTangent(unsigned int id) const {
	try {
		return pTriangle_->getTangent(id);
	}
	catch (const exception& kException) {
		throw runtime_error("Face.getTangent > " + string(kException.what()));
	}
}



const vec3* Face::getBitangent(unsigned int id) const {
	try {
		return pTriangle_->getBitangent(id);
	}
	catch (const exception& kException) {
		throw runtime_error("Face.getBitangent > " + string(kException.what()));
	}
}



bool Face::hasVertices() const {
	return pTriangle_->hasVertices();
}



bool Face::hasTexCoords() const {
	return pTriangle_->hasTexCoords();
}



bool Face::hasNormals() const {
	return pTriangle_->hasNormals();
}



bool Face::hasTangentsAndBitangents() const {
	return pTriangle_->hasTangentsAndBitangents();
}



bool Face::isTwoSided() const {
	if (pkMaterial_) return twoSided_ || pkMaterial_->isTwoSided();
	else return twoSided_;	
}



bool Face::isWireframe() const {
	if (pkMaterial_) return wireframe_ || pkMaterial_->isWireframe();
	else return wireframe_;
}



bool Face::isTransparent() const {
	if (pkMaterial_) return pkMaterial_->isTransparent();
	else return false;
}



const mat4* Face::getModelMatrix() const {
	return &modelMatrix_;
}



unsigned int Face::getId() const {
	return kId_;
}



unsigned int Face::getMeshId() const {
	return kMeshId_;
}



unsigned int Face::getModelId() const {
	return kModelId_;
}



string Face::toString() const {
	return to_string(kModelId_) + "_" + to_string(kMeshId_) + "_" + to_string(kId_);
}



void Face::updateModelMatrix_() {
	modelMatrix_ = (rotationSpeed_ != 0.0f) ? transformationMatrix_ * rotationMatrix_ : transformationMatrix_;
}
