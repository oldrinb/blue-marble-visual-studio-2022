/**
 * Author: Oldrin Barbulescu
 * Last modified: Feb 23, 2021
 **/

#ifndef MODEL_H
#define MODEL_H

#include "material/Material.h"
#include "mesh/mesh/Mesh.h"
#include "texture/texture/ColorTexture.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h> 

#include <glm/geometric.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <algorithm>
#include <cmath>
#include <exception>
#include <iomanip>
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>

using glm::mat4;
using glm::uvec3;
using glm::vec2;
using glm::vec3;

using std::cout;
using std::endl;
using std::exception;
using std::left;
using std::list;
using std::right;
using std::runtime_error;
using std::setw;
using std::string;
using std::to_string;



class Model3D {
public:
	Model3D(const string& kFilePath, const string& kFileName, unsigned int postProcessSteps, int removeComponents,
		    bool normalize, bool transparencySorting);
	~Model3D();


	// init: 1) loadMaterials
	//       2) [loadTextures]
	//       3) loadMeshes
	//       4) [loadTransformations]
	//############################################################################
	void loadMaterials(list<Material>& rMaterials);	
	void loadTextures(list<ColorTexture>& rDiffuseTextures,  list<ColorTexture>& rSpecularTextures, 
		              list<ColorTexture>& rEmissiveTextures, list<ColorTexture>& rNormalMapTextures);
	void loadMeshes(list<Mesh>& rMeshes);
	void loadTransformations();

private:
	static float getMaxSmoothingAngle_(); // degrees

	static const float kEpsilon_;
	static const float kLengthEpsilon_;
	static const unsigned int kLineLength_;

	static unsigned int modelId_;

	Model3D(const Model3D&);
	const Model3D& operator=(const Model3D&) {}

	void readModel_(unsigned int postProcessSteps, int removeComponents, bool normalize);

	string getMaterialName_(const aiMaterial* pkAiMaterial) const;

	string getMaterialShadingModel_(const aiMaterial* pkAiMaterial, Material::ShadingModel& rShadingModel) const;

	bool isMaterialTwoSided_(const aiMaterial* pkAiMaterial) const;
	bool isMaterialWireframe_(const aiMaterial* pkAiMaterial) const;
	float getMaterialOpacity_(const aiMaterial* pkAiMaterial) const;
	string getMaterialBlendMode_(const aiMaterial* pkAiMaterial) const;

	vec3 getMaterialAmbientColor_(const aiMaterial* pkAiMaterial) const;
	vec3 getMaterialDiffuseColor_(const aiMaterial* pkAiMaterial) const;
	vec3 getMaterialSpecularColor_(const aiMaterial* pkAiMaterial) const;
	vec3 getMaterialEmissiveColor_(const aiMaterial* pkAiMaterial) const;
	vec3 getMaterialTransparentColor_(const aiMaterial* pkAiMaterial) const;

	float getMaterialShininess_(const aiMaterial* pkAiMaterial) const;
	float getMaterialShininessStrength_(const aiMaterial* pkAiMaterial) const;

	float getMaterialRefraction_(const aiMaterial* pkAiMaterial) const;

	unsigned int getTextureCount_(const aiMaterial* pkAiMaterial, ColorTexture::TextureType textureType) const;

	const ColorTexture* getTexture_(const aiMaterial* pkAiMaterial, list<ColorTexture>& rTextures,
		                            ColorTexture::TextureType textureType, unsigned int textureNo) const;
	
	bool getTextureMappingMode_(const aiMaterial* pkAiMaterial, ColorTexture::TextureType textureType,
		                        unsigned int textureNo) const; // true -> GL_TEXTURE_WRAP_S = GL_TEXTURE_WRAP_T = GL_CLAMP_TO_EDGE

	void getMeshFaces_(const aiMesh* pkAiMesh, uvec3*& rpFaces, unsigned int& rnFaces) const;
	void getMeshVertices_(const aiMesh* pkAiMesh, vec3*& rpVertices, unsigned int& rnVertices) const;
	void getMeshTextureCoords_(const aiMesh* pkAiMesh, vec2*& rpTexCoords, unsigned int nVertices) const;
	void getMeshColors_(const aiMesh* pkAiMesh, vec3*& rpColors, unsigned int nVertices) const;
	void getMeshNormals_(const aiMesh* pkAiMesh, vec3*& rpNormals, unsigned int nVertices) const;
	void getMeshTangentsAndBitangents_(const aiMesh* pkAiMesh, const vec3* pkNormals,
		                               vec3*& rpTangents, vec3*& rpBitangents, unsigned int nVertices) const;

	void loadTransformations_(const aiNode* pkAiNode, const aiMatrix4x4& kTransformationMatrix);

	void displayModelInfo_() const;
	void displayMaterialInfo_() const;
	void displayTextureInfo_() const;
	void displayMeshInfo_() const;
	void displayNodeInfo_() const;

	void displayNodeInfo_(const aiNode* pkAiNode, unsigned int level) const;

	bool isIdentity_(const aiMatrix4x4& kMatrix) const;

	const aiScene* pkAiScene_;
	const aiNode* pkAiNode_;
	const string kFilePath_, kFileName_;

	Assimp::Importer importer_;

	list<Mesh>* pMeshes_;
	list<Material>* pMaterials_;

	bool transparencySorting_;
};

#endif
