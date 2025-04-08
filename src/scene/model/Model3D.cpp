/**
 * Author: Oldrin Barbulescu
 * Last modified: Apr 3, 2025
 **/

#include "Model3D.h"



float Model3D::getMaxSmoothingAngle_() { // degrees
	return 80.0f;
}



const float Model3D::kEpsilon_ = 0.001f;
const float Model3D::kLengthEpsilon_ = 0.01f;
const unsigned int Model3D::kLineLength_ = 96u;



unsigned int Model3D::modelId_ = 0;



Model3D::Model3D(const string& kFilePath, const string& kFileName, unsigned int postProcessSteps, int removeComponents,
	             bool normalize, bool transparencySorting):
	             pkAiScene_(nullptr), pkAiNode_(nullptr), kFilePath_(kFilePath), kFileName_(kFileName), importer_(),
	             pMeshes_(nullptr), pMaterials_(nullptr), transparencySorting_(transparencySorting) {
	try {
		readModel_(postProcessSteps, removeComponents, normalize);
		Model3D::modelId_++;
		pkAiNode_ = pkAiScene_->mRootNode;
	}
	catch (const exception& kException) {
		throw runtime_error("Model3D > " + string(kException.what()));
	}
	
	if (!pkAiScene_->HasMaterials() || pkAiScene_->mNumMaterials == 0u) {
		importer_.FreeScene();
		throw runtime_error("Model3D|Model is incomplete. There should be at least one material.");
	}

	if (!pkAiScene_->HasMeshes() || pkAiScene_->mNumMeshes == 0u) {
		importer_.FreeScene();
		throw runtime_error("Model3D|Model is incomplete. There should be at least one mesh.");
	}

	if (!pkAiNode_) {
		importer_.FreeScene();
		throw runtime_error("Model3D|Model is incomplete. There should be at least one node.");
	}

	//cout << "Model 3D created." << endl;
}



Model3D::~Model3D()  {
	importer_.FreeScene();

	//cout << "Model 3D deleted." << endl;
}



void Model3D::loadMaterials(list<Material>& rMaterials) {
	pMaterials_ = &rMaterials;

	for (unsigned int i = 0u; i < pkAiScene_->mNumMaterials; i++) {
		for (const Material& ikMaterial : *pMaterials_)
			if (ikMaterial.getModelId() == (Model3D::modelId_ - 1u) && ikMaterial.getId() == i)
				throw runtime_error("Model3D.loadMaterials|Material " + ikMaterial.toString() + " already loaded.");
		
		const aiMaterial* pkAiMaterial = pkAiScene_->mMaterials[i];
		if (!pkAiMaterial)
			throw runtime_error("Model3D.loadMaterials|Model is incomplete. Material " + to_string(i) + " is null.");		
		
		try {
			pMaterials_->emplace_back(i, Model3D::modelId_ - 1u);
			Material* pMaterial = &pMaterials_->back();

			Material::ShadingModel shadingModel;
			getMaterialShadingModel_(pkAiMaterial, shadingModel);
			pMaterial->setShadingModel(shadingModel);

			bool twoSided = isMaterialTwoSided_(pkAiMaterial);
			pMaterial->setTwoSided(twoSided);

			bool wireframe = isMaterialWireframe_(pkAiMaterial);
			pMaterial->setWireframe(wireframe);

			float opacity = getMaterialOpacity_(pkAiMaterial);
			pMaterial->setOpacity(opacity);

			vec3 color = getMaterialAmbientColor_(pkAiMaterial);
			pMaterial->setAmbientColor(color);

			color = getMaterialDiffuseColor_(pkAiMaterial);
			pMaterial->setDiffuseColor(color);

			color = getMaterialSpecularColor_(pkAiMaterial);
			pMaterial->setSpecularColor(color);

			color = getMaterialEmissiveColor_(pkAiMaterial);
			pMaterial->setEmissiveColor(color);

			float shininess = getMaterialShininess_(pkAiMaterial);
			pMaterial->setShininess(shininess / 4.0f);

			float strength = getMaterialShininessStrength_(pkAiMaterial);
			pMaterial->setShininessStrength(strength);
		}
		catch (const exception& kException) {
			throw runtime_error("Model3D.loadMaterials > " + string(kException.what()));
		}
	}

	displayMaterialInfo_();
}



void Model3D::loadTextures(list<ColorTexture>& rDiffuseTextures, list<ColorTexture>& rSpecularTextures,
	                       list<ColorTexture>& rEmissiveTextures, list<ColorTexture>& rNormalMapTextures) {
	if (!pMaterials_ || pMaterials_->size() == 0u)
		throw runtime_error("Model3D.loadTextures|Materials must be loaded before textures.");

	if (pMeshes_ && pMeshes_->size() > 0u)
		throw runtime_error("Model3D.loadTextures|Textures must be loaded before meshes.");

	for (Material& iMaterial : *pMaterials_) {
		if (iMaterial.getModelId() != Model3D::modelId_ - 1u) continue;
		const aiMaterial* pkAiMaterial = pkAiScene_->mMaterials[iMaterial.getId()];
		
		try {
			// only first texture in stack is implemented
			const ColorTexture* pkTexture = nullptr;					
			unsigned int texCount = getTextureCount_(pkAiMaterial, ColorTexture::TextureType::DIFFUSE);
			for (unsigned int i = 0u; i < texCount; i++) {
				cout << endl;
				pkTexture = getTexture_(pkAiMaterial, rDiffuseTextures, ColorTexture::TextureType::DIFFUSE, i);
				if (pkTexture) {
					iMaterial.setDiffuseTexture(pkTexture);
					break;
				}
			}

			pkTexture = nullptr;
			texCount = getTextureCount_(pkAiMaterial, ColorTexture::TextureType::SPECULAR);
			for (unsigned int i = 0u; i < texCount; i++) {
				pkTexture = getTexture_(pkAiMaterial, rSpecularTextures, ColorTexture::TextureType::SPECULAR, i);
				if (pkTexture) {
					iMaterial.setSpecularTexture(pkTexture);
					break;
				}
			}

			pkTexture = nullptr;
			texCount = getTextureCount_(pkAiMaterial, ColorTexture::TextureType::EMISSIVE);
			for (unsigned int i = 0u; i < texCount; i++) {
				pkTexture = getTexture_(pkAiMaterial, rEmissiveTextures, ColorTexture::TextureType::EMISSIVE, i);
				if (pkTexture) {
					iMaterial.setEmissiveTexture(pkTexture);
					break;
				}
			}

			pkTexture = nullptr;
			texCount = getTextureCount_(pkAiMaterial, ColorTexture::TextureType::NORMAL_MAP);
			for (unsigned int i = 0u; i < texCount; i++) {
				pkTexture = getTexture_(pkAiMaterial, rNormalMapTextures, ColorTexture::TextureType::NORMAL_MAP, i);
				if (pkTexture) {
					iMaterial.setNormalMapTexture(pkTexture);
					break;
				}
			}
		}
		catch (const exception& kException) {
			throw runtime_error("Model3D.loadTextures > " + string(kException.what()));
		}
	}

	displayTextureInfo_();
}



void Model3D::loadMeshes(list<Mesh>& rMeshes) {
	if (!pMaterials_ || pMaterials_->size() == 0u)
		throw runtime_error("Model3D.loadMeshes|Materials must be loaded before meshes.");

	pMeshes_ = &rMeshes;

	for (unsigned int i = 0u; i < pkAiScene_->mNumMeshes; i++) {
		for (const Mesh& ikMesh : *pMeshes_)
			if (ikMesh.getModelId() == (Model3D::modelId_ - 1u) && ikMesh.getId() == i)
				throw runtime_error("Model3D.loadMeshes|Mesh " + ikMesh.toString() + " already loaded.");
		
		const aiMesh* pkAiMesh = pkAiScene_->mMeshes[i];
		if (!pkAiMesh)
			throw runtime_error("Model3D.loadMeshes|Model is incomplete. Mesh " + to_string(i) + " is null.");		

		const Material* pkMaterial = nullptr;
		for (const Material& ikMaterial : *pMaterials_)
			if (ikMaterial.getModelId() == (Model3D::modelId_ - 1u) && ikMaterial.getId() == pkAiMesh->mMaterialIndex) {
				pkMaterial = &ikMaterial;
				break;
			}

		if (!pkMaterial) {
			Material mat(pkAiMesh->mMaterialIndex, Model3D::modelId_ - 1u);
			throw runtime_error("Model3D.loadMeshes|Material " + mat.toString() + " not found.");
		}
		
		uvec3* pFaces = nullptr;
		vec3* pVertices = nullptr;
		vec2* pTexCoords = nullptr;
		vec3* pNormals = nullptr;
		vec3* pTangents = nullptr;
		vec3* pBitangents = nullptr;
		
		try {
			unsigned int nFaces = 0u, nVertices = 0u;

			getMeshFaces_(pkAiMesh, pFaces, nFaces);

			if (pFaces) {
				getMeshVertices_(pkAiMesh, pVertices, nVertices);
				getMeshTextureCoords_(pkAiMesh, pTexCoords, nVertices);
				getMeshNormals_(pkAiMesh, pNormals, nVertices);

				if (pNormals)
					getMeshTangentsAndBitangents_(pkAiMesh, pNormals, pTangents, pBitangents, nVertices);

				pMeshes_->emplace_back(i, Model3D::modelId_ - 1u, pkMaterial, nFaces, nVertices, transparencySorting_);
				Mesh* pMesh = &pMeshes_->back();

				pMesh->loadFaces(pFaces);

				if (pVertices) pMesh->loadVertices(pVertices);
				if (pTexCoords) pMesh->loadTexCoords(pTexCoords);
				if (pNormals) {
					pMesh->loadNormals(pNormals);
					if (pTangents && pBitangents) pMesh->loadTangentsAndBitangents(pTangents, pBitangents);
				}
			}
		}
		catch (const exception& kException) {
			if (pFaces) delete[] pFaces;
			if (pVertices) delete[] pVertices;
			if (pTexCoords) delete[] pTexCoords;
			if (pNormals) delete[] pNormals;
			if (pTangents) delete[] pTangents;
			if (pBitangents) delete[] pBitangents;
			throw runtime_error("Model3D.loadMeshes > " + string(kException.what()));
		}

		if (pFaces) delete[] pFaces;
		if (pVertices) delete[] pVertices;
		if (pTexCoords) delete[] pTexCoords;
		if (pNormals) delete[] pNormals;
		if (pTangents) delete[] pTangents;
		if (pBitangents) delete[] pBitangents;
	}

	displayMeshInfo_();
}



void Model3D::loadTransformations() {
	if (!pMeshes_ || pMeshes_->size() == 0u)
		throw runtime_error("Model3D.loadTransformations|Meshes must be loaded before transformations.");
	if (pkAiNode_ != pkAiScene_->mRootNode)
		throw runtime_error("Model3D.loadTransformations|Transformations already loaded.");

	try {
		loadTransformations_(pkAiNode_, aiMatrix4x4());
	}
	catch (const exception& kException) {
		throw runtime_error("Model3D.loadTransformations > " + string(kException.what()));
	}

	displayNodeInfo_();
}



void Model3D::readModel_(unsigned int postProcessSteps, int removeComponents, bool normalize) {
	// configures the aiProcess_PreTransformVertices step
	importer_.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, static_cast<int>(normalize));
	importer_.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, removeComponents); // configures the aiProcess_RemoveComponent step
	// configures the aiProcess_GenSmoothNormals step
	importer_.SetPropertyFloat("PP_GSN_MAX_SMOOTHING_ANGLE", Model3D::getMaxSmoothingAngle_());


	if (!importer_.ValidateFlags(postProcessSteps))
		throw runtime_error("Model3D.readModel_|Assimp post processing flags are not valid.");

	pkAiScene_ = importer_.ReadFile(kFilePath_ + "/" + kFileName_, postProcessSteps);
	string error = importer_.GetErrorString();

	if (pkAiScene_ && error.empty()) {
		cout << endl << string(Model3D::kLineLength_, '#') << endl;
		cout << endl << "Import of model '" << kFileName_ << "' succeeded." << endl;
	}
	else {
		string errorMessage = "Model3D.readModel_|Cannot read '" + kFileName_ + "' file.";
		if (!error.empty()) errorMessage += "\n" + error;
		throw runtime_error(errorMessage);
	}
		
	if (((pkAiScene_->mFlags & AI_SCENE_FLAGS_VALIDATED) > 0u) ||
		((pkAiScene_->mFlags & AI_SCENE_FLAGS_NON_VERBOSE_FORMAT) > 0u)) {
		cout << endl;

		if ((pkAiScene_->mFlags & AI_SCENE_FLAGS_VALIDATED) > 0u)
			cout << "aiProcess_ValidateDataStructure: validation is successful." << endl;

		if ((pkAiScene_->mFlags & AI_SCENE_FLAGS_NON_VERBOSE_FORMAT) > 0u)
			cout << "aiProcess_JoinIdenticalVertices: the vertices of the output meshes aren't in the internal\nverbose format"
			" anymore." << endl;
	}

	if (((pkAiScene_->mFlags & AI_SCENE_FLAGS_INCOMPLETE) > 0u) ||
		((pkAiScene_->mFlags & AI_SCENE_FLAGS_VALIDATION_WARNING) > 0u) ||
		((pkAiScene_->mFlags & AI_SCENE_FLAGS_TERRAIN) > 0u)) {
		cout << endl << "Warning(s):" << endl;

		if ((pkAiScene_->mFlags & AI_SCENE_FLAGS_INCOMPLETE) > 0u)
			cout << "- scene data structure that was imported is not complete." << endl;

		if ((pkAiScene_->mFlags & AI_SCENE_FLAGS_VALIDATION_WARNING) > 0u)
			cout << "- aiProcess_ValidateDataStructure: validation is successful but some issues have been found." << endl;

		if ((pkAiScene_->mFlags & AI_SCENE_FLAGS_TERRAIN) > 0u)
			cout << "- pure height-map terrain data." << endl;
	}

	displayModelInfo_();
}



string Model3D::getMaterialName_(const aiMaterial* pkAiMaterial) const {
	aiString name;
	if(pkAiMaterial->Get(AI_MATKEY_NAME, name) == AI_SUCCESS)
		return name.C_Str();
	else return "";
}



string Model3D::getMaterialShadingModel_(const aiMaterial* pkAiMaterial, Material::ShadingModel& rShadingModel) const {
	int model;
	rShadingModel = Material::ShadingModel::GOURAUD;

	if (pkAiMaterial->Get(AI_MATKEY_SHADING_MODEL, model) == AI_SUCCESS) {
		switch (model) {
		case aiShadingMode_Flat: // not implemented
			return "Flat";
		case aiShadingMode_Gouraud:
		// illum 1 -> This is a diffuse illumination model using Lambertian shading. The color includes an ambient constant term
		// and a diffuse shading term for each light source. The formula is color = KaIa + Kd{ SUM j = 1..ls, (N * Lj)Ij }
			rShadingModel = Material::ShadingModel::GOURAUD;
			return "Gouraud";
		case aiShadingMode_Phong:
		// illum 2 -> This is a diffuse and specular illumination model using Lambertian shading and Blinn's interpretation
		// of Phong's specular illumination model(BLIN77). The color includes an ambient constant term, and a diffuse and specular
		// shading term for each light source. 
		// The formula is : color = KaIa + Kd{ SUM j = 1..ls, (N * Lj)Ij } +Ks{ SUM j = 1..ls, ((H * Hj) ^ Ns)Ij }
			rShadingModel = Material::ShadingModel::PHONG;
			return "Phong";
		case aiShadingMode_Blinn: // not implemented
			return "Phong-Blinn";
		case aiShadingMode_Toon: // not implemented
			return "Toon";
		case aiShadingMode_OrenNayar: // not implemented
			return "Oren-Nayar";
		case aiShadingMode_Minnaert: // not implemented
			return "Minnaert";
		case aiShadingMode_CookTorrance: // not implemented
			return "Cook-Torrance";
		case aiShadingMode_NoShading:
		// illum 0 -> This is a constant color illumination model. The color is the specified Kd for the material.
		// The formula is : color = Kd
			rShadingModel = Material::ShadingModel::NO_SHADING;
			return "No shading";
		case aiShadingMode_Fresnel: // not implemented
			return "Fresnel";
		}
	}

	return "-";
}



bool Model3D::isMaterialTwoSided_(const aiMaterial* pkAiMaterial) const {
	int twoSided = 0;
	return (pkAiMaterial->Get(AI_MATKEY_TWOSIDED, twoSided) == AI_SUCCESS) && (twoSided != 0);
}



bool Model3D::isMaterialWireframe_(const aiMaterial* pkAiMaterial) const {
	int wireframe = 0;
	return (pkAiMaterial->Get(AI_MATKEY_ENABLE_WIREFRAME, wireframe) == AI_SUCCESS) && (wireframe != 0);
}



float Model3D::getMaterialOpacity_(const aiMaterial* pkAiMaterial) const {
	float opacity = 1.0f; // default opacity
	pkAiMaterial->Get(AI_MATKEY_OPACITY, opacity);
	return opacity;
}



// not implemented
string Model3D::getMaterialBlendMode_(const aiMaterial* pkAiMaterial) const {
	int blend;

	if (pkAiMaterial->Get(AI_MATKEY_BLEND_FUNC, blend) == AI_SUCCESS) {
		switch (blend) {
		case aiBlendMode_Default:
			return "default";
		case aiBlendMode_Additive:
			return "additive";
		}
	}

	return "-";
}



vec3 Model3D::getMaterialAmbientColor_(const aiMaterial* pkAiMaterial) const {
	aiColor3D color(0.0f, 0.0f, 0.0f);
	pkAiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
	return vec3(color.r, color.g, color.b);
}



vec3 Model3D::getMaterialDiffuseColor_(const aiMaterial* pkAiMaterial) const {
	aiColor3D color(0.0f, 0.0f, 0.0f);
	pkAiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	return vec3(color.r, color.g, color.b);
}



vec3 Model3D::getMaterialSpecularColor_(const aiMaterial* pkAiMaterial) const {
	aiColor3D color(0.0f, 0.0f, 0.0f);
	pkAiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
	return vec3(color.r, color.g, color.b);
}



vec3 Model3D::getMaterialEmissiveColor_(const aiMaterial* pkAiMaterial) const {
	aiColor3D color(0.0f, 0.0f, 0.0f);
	pkAiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color);
	return vec3(color.r, color.g, color.b);
}



// not implemented
vec3 Model3D::getMaterialTransparentColor_(const aiMaterial* pkAiMaterial) const {
	aiColor3D color(0.0f, 0.0f, 0.0f);
	pkAiMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, color);
	return vec3(color.r, color.g, color.b);
}



float Model3D::getMaterialShininess_(const aiMaterial* pkAiMaterial) const {
	float shininess = 0.0f; // default shininess
	pkAiMaterial->Get(AI_MATKEY_SHININESS, shininess);
	return shininess;
}



float Model3D::getMaterialShininessStrength_(const aiMaterial* pkAiMaterial) const {
	float strength = 1.0f; // default shininess strength
	pkAiMaterial->Get(AI_MATKEY_SHININESS_STRENGTH, strength);
	return strength;
}



// not implemented
float Model3D::getMaterialRefraction_(const aiMaterial* pkAiMaterial) const {
	float refraction = 1.0f; // default refraction
	pkAiMaterial->Get(AI_MATKEY_REFRACTI, refraction);
	return refraction;
}



unsigned int Model3D::getTextureCount_(const aiMaterial* pkAiMaterial, ColorTexture::TextureType textureType) const {
	unsigned int texCount = 0u;

	switch (textureType) {
	case ColorTexture::TextureType::DIFFUSE:
		texCount = pkAiMaterial->GetTextureCount(aiTextureType_DIFFUSE);
		break;
	case ColorTexture::TextureType::SPECULAR:
		texCount = pkAiMaterial->GetTextureCount(aiTextureType_SPECULAR);
		break;
	case ColorTexture::TextureType::EMISSIVE:
		texCount = pkAiMaterial->GetTextureCount(aiTextureType_EMISSIVE);
		break;
	case ColorTexture::TextureType::NORMAL_MAP:
		texCount = pkAiMaterial->GetTextureCount(aiTextureType_NORMALS);
	case ColorTexture::TextureType::NONE:
		break;
	}

	return texCount;
}



const ColorTexture* Model3D::getTexture_(const aiMaterial* pkAiMaterial, list<ColorTexture>& rTextures,
	                                     ColorTexture::TextureType textureType, unsigned int textureNo) const {
	aiString texPath;
	bool success = false;

	switch (textureType) {
	case ColorTexture::TextureType::DIFFUSE:
		if (pkAiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, textureNo), texPath) == AI_SUCCESS)
			success = true;
		break;
	case ColorTexture::TextureType::SPECULAR:
		if (pkAiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, textureNo), texPath) == AI_SUCCESS)
			success = true;
		break;
	case ColorTexture::TextureType::EMISSIVE:
		if (pkAiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_EMISSIVE, textureNo), texPath) == AI_SUCCESS)
			success = true;
		break;
	case ColorTexture::TextureType::NORMAL_MAP:
		if (pkAiMaterial->Get(AI_MATKEY_TEXTURE(aiTextureType_NORMALS, textureNo), texPath) == AI_SUCCESS)
			success = true;
	case ColorTexture::TextureType::NONE:
		break;
	}
	
	const ColorTexture* pkTexture = nullptr;

	if (success) {
		string fileName = texPath.data;

		for (const ColorTexture& ikTexture : rTextures)
			if (ikTexture.getFileName() == fileName) {
				pkTexture = &ikTexture;
				break;
			}

		if (!pkTexture) {
			try {
				bool clampToEdge = getTextureMappingMode_(pkAiMaterial, textureType, textureNo);
				bool linearFiltering = true, mipmapping = true;

				rTextures.emplace_back(kFilePath_, fileName, !clampToEdge, linearFiltering, mipmapping);
				pkTexture = &rTextures.back();

				if (pkTexture->isDDS())
					cout << "DDS texture '" << fileName << "' loaded." << endl;
				else cout << "Texture '" << fileName << "' loaded." << endl;

			}
			catch (const exception& kException) {
				throw runtime_error("Model3D.getTexture_ > " + string(kException.what()));
			}
		}
	}

	return pkTexture;
}



// only 'clamp to edges' is implemented, default: 'repeat'
bool Model3D::getTextureMappingMode_(const aiMaterial* pkAiMaterial,
	                                 ColorTexture::TextureType textureType, unsigned int textureNo) const {
	bool success = false;
	int mappingModeU = 0, mappingModeV = 0;

	switch (textureType) {
	case ColorTexture::TextureType::DIFFUSE:
		if (pkAiMaterial->Get(AI_MATKEY_MAPPINGMODE_U(aiTextureType_DIFFUSE, textureNo), mappingModeU) == AI_SUCCESS &&
			pkAiMaterial->Get(AI_MATKEY_MAPPINGMODE_V(aiTextureType_DIFFUSE, textureNo), mappingModeV) == AI_SUCCESS)
			success = true;
		break;
	case ColorTexture::TextureType::SPECULAR:
		if (pkAiMaterial->Get(AI_MATKEY_MAPPINGMODE_U(aiTextureType_SPECULAR, textureNo), mappingModeU) == AI_SUCCESS &&
			pkAiMaterial->Get(AI_MATKEY_MAPPINGMODE_V(aiTextureType_SPECULAR, textureNo), mappingModeV) == AI_SUCCESS)
			success = true;
		break;
	case ColorTexture::TextureType::EMISSIVE:
		if (pkAiMaterial->Get(AI_MATKEY_MAPPINGMODE_U(aiTextureType_EMISSIVE, textureNo), mappingModeU) == AI_SUCCESS &&
			pkAiMaterial->Get(AI_MATKEY_MAPPINGMODE_V(aiTextureType_EMISSIVE, textureNo), mappingModeV) == AI_SUCCESS)
			success = true;
		break;
	case ColorTexture::TextureType::NORMAL_MAP:
		if (pkAiMaterial->Get(AI_MATKEY_MAPPINGMODE_U(aiTextureType_NORMALS, textureNo), mappingModeU) == AI_SUCCESS &&
			pkAiMaterial->Get(AI_MATKEY_MAPPINGMODE_V(aiTextureType_NORMALS, textureNo), mappingModeV) == AI_SUCCESS)
			success = true;
	case ColorTexture::TextureType::NONE:
		break;
	}

	if (success && mappingModeU == mappingModeV == aiTextureMapMode_Clamp)
		return true;
	return false;
}



// only triangles are implemented
void Model3D::getMeshFaces_(const aiMesh* pkAiMesh, uvec3*& rpFaces, unsigned int& rnFaces) const {
	if (pkAiMesh->HasFaces() && pkAiMesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE) {
		rnFaces = pkAiMesh->mNumFaces;
		rpFaces = new uvec3[rnFaces];

		for (unsigned int i = 0u; i < rnFaces; i++) {
			const aiFace* pkAiFace = &pkAiMesh->mFaces[i];

			if (pkAiFace->mNumIndices == 3u) {
				rpFaces[i].x = pkAiFace->mIndices[0u];
				rpFaces[i].y = pkAiFace->mIndices[1u];
				rpFaces[i].z = pkAiFace->mIndices[2u];
			}
		}
	}
}



void Model3D::getMeshVertices_(const aiMesh* pkAiMesh, vec3*& rpVertices, unsigned int& rnVertices) const {
	if (pkAiMesh->HasPositions()) {
		rnVertices = pkAiMesh->mNumVertices;
		rpVertices = new vec3[rnVertices];

		for (unsigned int i = 0u; i < rnVertices; i++) {
			rpVertices[i].x = pkAiMesh->mVertices[i].x;
			rpVertices[i].y = pkAiMesh->mVertices[i].y;
			rpVertices[i].z = pkAiMesh->mVertices[i].z;
		}
	}
}



// only first layer of 2D ('uv') texture is implemented
void Model3D::getMeshTextureCoords_(const aiMesh* pkAiMesh, vec2*& rpTexCoords, unsigned int nVertices) const {
	for (unsigned int i = 0u; i < pkAiMesh->GetNumUVChannels(); i++)
		if (pkAiMesh->HasTextureCoords(i) && pkAiMesh->mNumUVComponents[i] == 2u) {
			rpTexCoords = new vec2[nVertices];

			for (unsigned int j = 0u; j < nVertices; j++) {
				rpTexCoords[j].x = pkAiMesh->mTextureCoords[i][j].x;
				rpTexCoords[j].y = pkAiMesh->mTextureCoords[i][j].y;
			}
			break;
		}
}



// not implemented
void Model3D::getMeshColors_(const aiMesh* pkAiMesh, vec3*& rpColors, unsigned int nVertices) const {
	for (unsigned int i = 0u; i < pkAiMesh->GetNumColorChannels(); i++)
		if (pkAiMesh->HasVertexColors(i)) {
			rpColors = new vec3[nVertices];
			break;
		}
}



void Model3D::getMeshNormals_(const aiMesh* pkAiMesh, vec3*& rpNormals, unsigned int nVertices) const {
	if (pkAiMesh->HasNormals()) {
		rpNormals = new vec3[nVertices];

		for (unsigned int i = 0u; i < nVertices; i++) {
			rpNormals[i].x = pkAiMesh->mNormals[i].x;
			rpNormals[i].y = pkAiMesh->mNormals[i].y;
			rpNormals[i].z = pkAiMesh->mNormals[i].z;

			float len = glm::length(rpNormals[i]);
			 
			if (len < Model3D::kLengthEpsilon_)
				throw runtime_error("Model3D.getMeshNormals_|Invalid normal vector.");

			if (std::abs(1.0f - len) > Model3D::kEpsilon_)
				rpNormals[i] = glm::normalize(rpNormals[i]);			
		}
	}
}



void Model3D::getMeshTangentsAndBitangents_(const aiMesh* pkAiMesh, const vec3* pkNormals,
	                                        vec3*& rpTangents, vec3*& rpBitangents, unsigned int nVertices) const {
	if (pkAiMesh->HasNormals() && pkAiMesh->HasTangentsAndBitangents()) {
		rpTangents = new vec3[nVertices];
		rpBitangents = new vec3[nVertices];

		for (unsigned int i = 0u; i < nVertices; i++) {
			rpTangents[i].x = pkAiMesh->mTangents[i].x;
			rpTangents[i].y = pkAiMesh->mTangents[i].y;
			rpTangents[i].z = pkAiMesh->mTangents[i].z;

			float len = glm::length(rpTangents[i]);

			if (len < Model3D::kLengthEpsilon_)
				throw runtime_error("Model3D.getMeshTangentsAndBitangents_|Invalid tangent vector.");

			if (std::abs(1.0f - len) > Model3D::kEpsilon_)
				rpTangents[i] = glm::normalize(rpTangents[i]);			

			rpBitangents[i].x = pkAiMesh->mBitangents[i].x;
			rpBitangents[i].y = pkAiMesh->mBitangents[i].y;
			rpBitangents[i].z = pkAiMesh->mBitangents[i].z;

			len = glm::length(rpBitangents[i]);

			if (len < Model3D::kLengthEpsilon_)
				throw runtime_error("Model3D.getMeshTangentsAndBitangents_|Invalid bitangent vector.");

			if (std::abs(1.0f - len) > Model3D::kEpsilon_)
				rpBitangents[i] = glm::normalize(rpBitangents[i]);			

			if (glm::dot(glm::cross(pkNormals[i], rpTangents[i]), rpBitangents[i]) < 0.0f)
				rpBitangents[i] = -rpBitangents[i];
		}
	}
}



void Model3D::loadTransformations_(const aiNode* pkAiNode, const aiMatrix4x4& kTransformationMatrix) {
	aiMatrix4x4 matrix = kTransformationMatrix * pkAiNode->mTransformation;

	for (unsigned int i = 0u; i < pkAiNode->mNumMeshes; i++) {
		unsigned int id = pkAiNode->mMeshes[i];

		for (Mesh& iMesh : *pMeshes_)
			try {
				if (iMesh.getModelId() == (Model3D::modelId_ - 1u) && iMesh.getId() == id) {
					iMesh.transform(mat4(matrix[0u][0u], matrix[1u][0u], matrix[2u][0u], matrix[3u][0u],
										 matrix[0u][1u], matrix[1u][1u], matrix[2u][1u], matrix[3u][1u],
										 matrix[0u][2u], matrix[1u][2u], matrix[2u][2u], matrix[3u][2u],
										 matrix[0u][3u], matrix[1u][3u], matrix[2u][3u], matrix[3u][3u]));
					break;
				}
			}
			catch (const exception& kException) {
				throw runtime_error("Model3D.loadTransformations_ > " + string(kException.what()));
			}
	}

	for (unsigned int i = 0u; i < pkAiNode->mNumChildren; i++) {
		pkAiNode_ = pkAiNode->mChildren[i];
		loadTransformations_(pkAiNode_, matrix);
	}
}



void Model3D::displayModelInfo_() const {
	unsigned int meshes = (pkAiScene_->HasMeshes()) ? pkAiScene_->mNumMeshes : 0u;
	unsigned int materials = (pkAiScene_->HasMaterials()) ? pkAiScene_->mNumMaterials : 0u;

	// not implemented.
	unsigned int textures = (pkAiScene_->HasTextures()) ? pkAiScene_->mNumTextures : 0u;

	// not implemented.
	unsigned int cameras = (pkAiScene_->HasCameras()) ? pkAiScene_->mNumCameras : 0u;

	// not implemented.
	unsigned int lights = (pkAiScene_->HasLights()) ? pkAiScene_->mNumLights : 0u;

	// not implemented.
	unsigned int animations = (pkAiScene_->HasAnimations()) ? pkAiScene_->mNumAnimations : 0u;

	cout << endl << "Model:" << endl;
	cout << "meshes  materials (textures) (cameras) (lights) (animations)" << endl;
	cout << string (Model3D::kLineLength_, '-') << endl;
	cout << right << setw(6) << meshes << "  " << setw(9) << materials << "  " << setw(8) << textures << "   "
		 << setw(7) << cameras << "   " << setw(6) << lights << "   " << setw(10) << animations << endl;
}



void Model3D::displayMaterialInfo_() const {
	cout << endl << "Materials:" << endl;
	cout << "   id  name                            shading model  twosided  wireframe  opacity (blend mode)" << endl;
	cout << string(Model3D::kLineLength_, '-') << endl;
	
	for (const Material& ikMaterial : *pMaterials_) {
		if (ikMaterial.getModelId() != Model3D::modelId_ - 1u) continue;
		const aiMaterial* pkAiMaterial = pkAiScene_->mMaterials[ikMaterial.getId()];

		cout << setw(5) << right << ikMaterial.toString().substr(0u, 5u) << "  ";

		string name = getMaterialName_(pkAiMaterial);
		if (name.length() > 30u) name = name.substr(0u, 27u) + "...";
		cout << setw(30) << left << name << "  ";

		Material::ShadingModel shadingModel;
		string sm = getMaterialShadingModel_(pkAiMaterial, shadingModel);
		cout << setw(13) << left << sm.substr(0u, 13u) << "  ";

		cout << setw(8) << left << (ikMaterial.isTwoSided() ? "yes" : "no") << "  ";
		cout << setw(9) << left << (ikMaterial.isWireframe() ? "yes" : "no") << "  ";

		cout << setw(7) << right << std::fixed << std::setprecision(2) << ikMaterial.getOpacity() << " ";

		// not implemented
		string blendMode = getMaterialBlendMode_(pkAiMaterial);
		cout << " " << setw(10) << left << blendMode.substr(0u, 10u) << endl;
	}

	cout << endl;
	cout << "   id  ambient  diffuse  specular  emissive (transparent) shininess  strength (refraction)" << endl;
	cout << string(Model3D::kLineLength_, '-') << endl;

	for (const Material& ikMaterial : *pMaterials_) {
		if (ikMaterial.getModelId() != Model3D::modelId_ - 1u) continue;
		const aiMaterial* pkAiMaterial = pkAiScene_->mMaterials[ikMaterial.getId()];

		cout << setw(5) << right << ikMaterial.toString().substr(0u, 5u) << "  ";

		vec3 color = *ikMaterial.getAmbientColor();
		cout << setw(7) << left << (color.r + color.g + color.b > 0.0f ? "yes" : "no") << "  ";
		
		color = *ikMaterial.getDiffuseColor();
		cout << setw(7) << left << (color.r + color.g + color.b > 0.0f ? "yes" : "no") << "  ";

		color = *ikMaterial.getSpecularColor();
		cout << setw(8) << left << (color.r + color.g + color.b > 0.0f ? "yes" : "no") << "  ";

		color = *ikMaterial.getEmissiveColor();
		cout << setw(8) << left << (color.r + color.g + color.b > 0.0f ? "yes" : "no") << " ";

		// not implemented
		color = getMaterialTransparentColor_(pkAiMaterial);
		cout << " " << setw(11) << left << (color.r + color.g + color.b > 0.0f ? "yes" : "no") << "  ";

		cout << setw(9) << right << std::fixed << std::setprecision(2) << ikMaterial.getShininess() << "  ";

		cout << setw(8) << right << std::fixed << std::setprecision(2) << ikMaterial.getShininessStrength() << " ";
			
		// not implemented
		float refraction = getMaterialRefraction_(pkAiMaterial);
		cout << " " << setw(10) << right << std::fixed << std::setprecision(2) << refraction << endl;
	}
}



void Model3D::displayTextureInfo_() const {
	cout << endl << "Textures:" << endl;
	cout << "mat id (ambient) diffuse  specular  emissive  normals (height) (shininess) (opacity) (others)" << endl;
	cout << string(Model3D::kLineLength_, '-') << endl;

	for (const Material& ikMaterial : *pMaterials_) {
		if (ikMaterial.getModelId() != Model3D::modelId_ - 1u) continue;
		const aiMaterial* pkAiMaterial = pkAiScene_->mMaterials[ikMaterial.getId()];

		cout << right;
		cout << setw(6) << ikMaterial.toString().substr(0u, 6u) << " ";

		unsigned int t = pkAiMaterial->GetTextureCount(aiTextureType_AMBIENT);// not implemented
		cout << " " << setw(7) << t << "  ";
		t = pkAiMaterial->GetTextureCount(aiTextureType_DIFFUSE); cout << setw(7) << t << "  ";
		t = pkAiMaterial->GetTextureCount(aiTextureType_SPECULAR); cout << setw(8) << t << "  ";
		t = pkAiMaterial->GetTextureCount(aiTextureType_EMISSIVE); cout << setw(8) << t << "  ";
		t = pkAiMaterial->GetTextureCount(aiTextureType_NORMALS); cout << setw(7) << t << " ";
		t = pkAiMaterial->GetTextureCount(aiTextureType_HEIGHT); cout << " " << setw(6) << t << "  ";// not implemented
		t = pkAiMaterial->GetTextureCount(aiTextureType_SHININESS); cout << " " << setw(9) << t << "  ";// not implemented
		t = pkAiMaterial->GetTextureCount(aiTextureType_OPACITY); cout << " " << setw(7) << t << "  ";// not implemented

		// not implemented
		t = pkAiMaterial->GetTextureCount(aiTextureType_NONE);
		t += pkAiMaterial->GetTextureCount(aiTextureType_DISPLACEMENT);
		t += pkAiMaterial->GetTextureCount(aiTextureType_LIGHTMAP);
		t += pkAiMaterial->GetTextureCount(aiTextureType_REFLECTION);
		t += pkAiMaterial->GetTextureCount(aiTextureType_UNKNOWN);
		cout << " " << setw(6) << t << endl;
	}
}



void Model3D::displayMeshInfo_() const {
	cout << endl << "Meshes:" << endl;
	cout << "   id  name                            mat id  faces   vert. texture (colors) norm.  tg. (bones)" << endl;
	cout << string(Model3D::kLineLength_, '-') << endl;

	for (const Mesh& ikMesh : *pMeshes_) {
		if (ikMesh.getModelId() != Model3D::modelId_ - 1u) continue;
		const aiMesh* pkAiMesh = pkAiScene_->mMeshes[ikMesh.getId()];

		cout << setw(5) << right << ikMesh.toString().substr(0u, 5u) << "  ";

		string name = pkAiMesh->mName.C_Str();
		if (name.length() > 30u) name = name.substr(0u, 27u) + "...";
		cout << setw(30) << left << name << "  ";

		const Material* pkMaterial = ikMesh.getMaterial();
		if (pkMaterial)
			cout << setw(6) << right << pkMaterial->toString().substr(0u, 6u) << " ";			
		else cout << "       ";

		// only triangles are implemented.
		cout << setw(6) << right << std::min(ikMesh.getNumFaces(), 999999u) << " ";

		cout << setw(6) << right << std::min(ikMesh.getNumVertices(), 999999u) << "  ";

		cout << setw(7) << left << (ikMesh.hasTexCoords() ? "yes" : "no") << " ";
		
		// not implemented
		vec3* pColors = nullptr;
		getMeshColors_(pkAiMesh, pColors, ikMesh.getNumVertices());
		cout << " " << setw(6) << left << (pColors ? "yes" : "no") << "  ";
		if (pColors) delete[] pColors;
		
		cout << setw(5) << left << (ikMesh.hasNormals() ? "yes" : "no") << "  ";

		cout << setw(3) << left << (ikMesh.hasTangentsAndBitangents() ? "yes" : "no") << " ";
		
		// not implemented
		bool hasBones = pkAiMesh->HasBones() && pkAiMesh->mNumBones > 0u;
		cout << " " << setw(5) << left << (hasBones ? "yes" : "no") << endl;
	}
}



void Model3D::displayNodeInfo_() const {
	cout << endl << "Nodes:" << endl;
	cout << "name                                      children  transformation  meshes  mesh id" << endl;
	cout << string(Model3D::kLineLength_, '-') << endl;


	const aiNode* pkAiNode = pkAiScene_->mRootNode;
	unsigned int level = 0u;

	displayNodeInfo_(pkAiNode, level);
	cout << endl << endl;
}



void Model3D::displayNodeInfo_(const aiNode* pkAiNode, unsigned int level) const {
	string name = pkAiNode->mName.C_Str();
	unsigned int maxLength = 40u - 2u * level;

	for (unsigned int i = 0u; i < level; i++) cout << ": ";	
	if (name.length() > maxLength) name = name.substr(0u, maxLength - 3u) + "...";
	cout << setw(maxLength) << left << name << "  ";
	
	cout << setw(8) << right << pkAiNode->mNumChildren << "  ";

	cout << setw(14) << left << ((isIdentity_(pkAiNode->mTransformation)) ? "no" : "yes") << "  ";

	cout << setw(6) << right << pkAiNode->mNumMeshes << "  ";		
	
	string meshId = "";

	for (unsigned int i = 0u; i < pkAiNode->mNumMeshes; i++) {
		unsigned int id = pkAiNode->mMeshes[i];

		for (const Mesh& ikMesh : *pMeshes_)
			if (ikMesh.getModelId() == (Model3D::modelId_ - 1u) && ikMesh.getId() == id)
				meshId += ikMesh.toString() + ", ";		
	}
	if (!meshId.empty())
		meshId = meshId.substr(0u, meshId.length() - 2u);

	if (meshId.length() > 20u) meshId = meshId.substr(0u, 17u) + "...";
	if (meshId.length() <= 7u) cout << setw(7) << right << meshId << endl;
	else cout << left << meshId << endl;

	for (unsigned int i = 0u; i < pkAiNode->mNumChildren; i++)
		displayNodeInfo_(pkAiNode->mChildren[i], level + 1u);
}



bool Model3D::isIdentity_(const aiMatrix4x4& kMatrix) const {
	float sum = 0.0f;
	for (unsigned int i = 0; i < 4u; i++)
		for (unsigned int j = 0; j < 4u; j++)
			if (i == j) sum += std::abs(kMatrix[i][i] - 1.0f);
			else sum += std::abs(kMatrix[i][j]);
		
	return sum <= Model3D::kEpsilon_;
}
