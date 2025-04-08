/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 15, 2021
 **/

#include "Text2D.h"



unsigned int Text2D::getMaxTextLength_() {
	return 3500u;
}



unsigned int Text2D::getTextWidth_() {
	return 8u;
}



unsigned int Text2D::getTextHeight_() {
	return 16u;
}



unsigned int Text2D::getTextureSize_() {
	return 256u;
}



Text2D::Text2D(const uvec2& kWindowSize): nTotalVertices_(0u), pTriangleList2D_(nullptr), windowSize_(kWindowSize),
               pPosition_(nullptr), pVertices_(nullptr), pTextureCoords_(nullptr), pColors_ (nullptr),
	           nbCharU_(static_cast<float>(Text2D::getTextureSize_()) / static_cast<float>(Text2D::getTextWidth_())),
	           nbCharV_(static_cast<float>(Text2D::getTextureSize_()) / static_cast<float>(Text2D::getTextHeight_())) {

	if (kWindowSize.x == 0u || kWindowSize.y == 0u)
		throw runtime_error("Text2D|Invalid window size value.");

	pTriangleList2D_ = new TriangleList2D(3u);

	//cout << "Text2D created." << endl;
}



Text2D::~Text2D() {
	delete pTriangleList2D_;
	clearText_();

	//cout << "Text2D deleted." << endl;
}



void Text2D::addText(const string& kText, const vec2& kPosition, const vec3& kColor, bool bold) {
	addText(kText, kPosition, kColor, vec4(0.0f), bold);
}



// row, col = 0,1,...-2,-1; offset = -0.5f ... 0.5f (left, right, up, down)
void Text2D::addText(const string& kText, const vec2& kPosition, const vec3& kColor, const vec4& kOffset, bool bold) {
	unsigned int nVertices = 6u * static_cast<unsigned int>(kText.size());
	if (nVertices == 0u) throw runtime_error("Text2D.addText|Text cannot be empty.");

	try {
		vec2 textCoords = getTextCoordinates_(kPosition.x, kPosition.y);

		addPosition_(kPosition, nVertices);
		addVertices_(textCoords, nVertices, glm::clamp(kOffset, -0.5f, 0.5f));
		addTextureCoords_(kText, nVertices, glm::clamp(kOffset, -0.5f, 0.5f), bold);
		addColors_(glm::clamp(kColor, 0.0f, 1.0f), nVertices);
	}
	catch (const exception& kException) {
		throw runtime_error("Text2D.addText > " + string(kException.what()));
	}

	nTotalVertices_ += nVertices;
	if (nTotalVertices_ > 6u * Text2D::getMaxTextLength_())
		throw runtime_error("Text2D.addText|Maximum text length exceeded.");
}



void Text2D::updateVertexBuffer() {
	if (nTotalVertices_ > 0u) {
		try {
			pTriangleList2D_->setNumVertices(nTotalVertices_);
			pTriangleList2D_->loadVertices(pVertices_);
			pTriangleList2D_->loadTexCoords(pTextureCoords_);
			pTriangleList2D_->loadColors(pColors_);

			pTriangleList2D_->updateVertexBuffer();

			clearText_();
		}
		catch (const exception& kException) {
			throw runtime_error("Text2D.updateVertexBuffer > " + string(kException.what()));
		}
	}
}



void Text2D::setAttribPointers(unsigned int programId, GLuint positionIndex, GLuint texCoordIndex, GLuint colorIndex) {
	pTriangleList2D_->setAttribPointers(programId, positionIndex, texCoordIndex, colorIndex);
}



void Text2D::setWindowSize(const uvec2& kSize) {
	if (kSize.x == 0u || kSize.y == 0u)
		throw runtime_error("Text2D.setWindowSize|Invalid window size value.");
	windowSize_ = kSize;
}



void Text2D::render(unsigned int programId) const {
	try {
		pTriangleList2D_->render(programId);
	}
	catch (const exception& kException) {
		throw runtime_error("Text2D.render > " + string(kException.what()));
	}
}



void Text2D::clearText_() {
	nTotalVertices_ = 0u;

	if (pPosition_) delete[] pPosition_; pPosition_ = nullptr;
	if (pVertices_) delete[] pVertices_; pVertices_ = nullptr;
	if (pTextureCoords_) delete[] pTextureCoords_; pTextureCoords_ = nullptr;
	if (pColors_) delete[] pColors_; pColors_ = nullptr;
}



void Text2D::addPosition_(const vec2& kPosition, unsigned int nVertices) {
	if (nTotalVertices_ > 0u) {
		vec2* pTempPosition = pPosition_;
		pPosition_ = new vec2[(static_cast<size_t>(nTotalVertices_) + nVertices) / 6u];

		try {
			std::copy(pTempPosition, pTempPosition + nTotalVertices_ / 6u, pPosition_);
		}
		catch (const exception& kException) {
			delete[] pTempPosition;
			throw runtime_error("Text2D.addPosition_ > copy > " + string(kException.what()));
		}

		delete[] pTempPosition;
	}
	else pPosition_ = new vec2[nVertices / 6u];

	for (unsigned int i = 0u; i < nVertices / 6u; i++)
		pPosition_[nTotalVertices_ / 6u + i] = kPosition;
}



void Text2D::addVertices_(const vec2& kTextCoords, unsigned int nVertices, const vec4& kOffset) {
	if (nTotalVertices_ > 0u) {
		vec2* pTempVertices = pVertices_;
		pVertices_ = new vec2[static_cast<size_t>(nTotalVertices_) + nVertices];

		try {
			std::copy(pTempVertices, pTempVertices + nTotalVertices_, pVertices_);
		}
		catch (const exception& kException) {
			delete[] pTempVertices;
			throw runtime_error("Text2D.addVertices_ > copy > " + string(kException.what()));
		}

		delete[] pTempVertices;
	}
	else pVertices_ = new vec2[nVertices];

	for (unsigned int i = 0u; i < nVertices / 6u; i++) {
		vec2 upLeftV = vec2(static_cast<float>(Text2D::getTextWidth_()) * (static_cast<float>(i) + kOffset.x),
			                static_cast<float>(Text2D::getTextHeight_()) * (1.0f - kOffset.z));
		vec2 upRightV = vec2(static_cast<float>(Text2D::getTextWidth_()) * (static_cast<float>(i) + 1.0f - kOffset.y),
			                 static_cast<float>(Text2D::getTextHeight_()) * (1.0f - kOffset.z));
		vec2 downLeftV = vec2(static_cast<float>(Text2D::getTextWidth_()) * (static_cast<float>(i) + kOffset.x), 
			                  static_cast<float>(Text2D::getTextHeight_()) * kOffset.w);
		vec2 downRightV = vec2(static_cast<float>(Text2D::getTextWidth_()) * (static_cast<float>(i) + 1.0f - kOffset.y),
			                   static_cast<float>(Text2D::getTextHeight_()) * kOffset.w);

		pVertices_[nTotalVertices_ + 6u * i] = kTextCoords + upLeftV;
		pVertices_[nTotalVertices_ + 6u * i + 1u] = kTextCoords + downLeftV;
		pVertices_[nTotalVertices_ + 6u * i + 2u] = kTextCoords + upRightV;

		pVertices_[nTotalVertices_ + 6u * i + 3u] = kTextCoords + downRightV;
		pVertices_[nTotalVertices_ + 6u * i + 4u] = kTextCoords + upRightV;
		pVertices_[nTotalVertices_ + 6u * i + 5u] = kTextCoords + downLeftV;
	}
}



void Text2D::addTextureCoords_(const string& kText, unsigned int nVertices, const vec4& kOffset, bool bold) {
	if (nTotalVertices_ > 0u) {
		vec2* pTempTextureCoords = pTextureCoords_;
		pTextureCoords_ = new vec2[static_cast<size_t>(nTotalVertices_) + nVertices];

		try {
			std::copy(pTempTextureCoords, pTempTextureCoords + nTotalVertices_, pTextureCoords_);
		}
		catch (const exception& kException) {
			delete[] pTempTextureCoords;
			throw runtime_error("Text2D.addTextureCoords_ > copy > " + string(kException.what()));
		}

		delete[] pTempTextureCoords;
	}
	else pTextureCoords_ = new vec2[nVertices];

	for (unsigned int i = 0u; i < nVertices / 6u; i++) {
		char character = kText[i];
		float u = static_cast<float>(character % static_cast<unsigned int>(nbCharU_)) / nbCharU_;
		float v = static_cast<float>(character / static_cast<unsigned int>(nbCharU_)) / nbCharV_;
		if (bold) v += 0.5f;

		vec2 upLeftT = vec2(u + kOffset.x / nbCharU_, v + kOffset.z / nbCharV_);
		vec2 upRightT = vec2(u + (1.0f - kOffset.y) / nbCharU_, v + kOffset.z / nbCharV_);
		vec2 downLeftT = vec2(u + kOffset.x / nbCharU_, v + (1.0f - kOffset.w) / nbCharV_);
		vec2 downRightT = vec2(u + (1.0f - kOffset.y) / nbCharU_, v + (1.0f - kOffset.w) / nbCharV_);

		pTextureCoords_[nTotalVertices_ + 6u * i] = upLeftT;
		pTextureCoords_[nTotalVertices_ + 6u * i + 1u] = downLeftT;
		pTextureCoords_[nTotalVertices_ + 6u * i + 2u] = upRightT;

		pTextureCoords_[nTotalVertices_ + 6u * i + 3u] = downRightT;
		pTextureCoords_[nTotalVertices_ + 6u * i + 4u] = upRightT;
		pTextureCoords_[nTotalVertices_ + 6u * i + 5u] = downLeftT;
	}
}



void Text2D::addColors_(const vec3& kColor, unsigned int nVertices) {
	if (nTotalVertices_ > 0u) {
		vec3* pTempColors = pColors_;
		pColors_ = new vec3[static_cast<size_t>(nTotalVertices_) + nVertices];

		try {
			std::copy(pTempColors, pTempColors + nTotalVertices_, pColors_);
		}
		catch (const exception& kException) {
			delete[] pTempColors;
			throw runtime_error("Text2D.addColors_ > copy > " + string(kException.what()));
		}

		delete[] pTempColors;
	}
	else pColors_ = new vec3[nVertices];

	for (unsigned int i = 0u; i < nVertices / 6u; i++)
		for (unsigned int j = 0u; j < 6u; j++)
			pColors_[nTotalVertices_ + 6u * i + j] = kColor;
}



vec2 Text2D::getTextCoordinates_(float row, float col) const {
	if (std::abs (row) > static_cast<float>(windowSize_.y) / static_cast<float>(Text2D::getTextHeight_()))
		throw runtime_error("Text2D.getTextCoordinates_|Invalid row number value.");

	if (std::abs(col) > static_cast<float>(windowSize_.x) / static_cast<float>(Text2D::getTextWidth_()))
		throw runtime_error("Text2D.getTextCoordinates_|Invalid column number value.");

	float x = static_cast<float>(Text2D::getTextWidth_()) * col;
	if (col < 0.0f) x += static_cast<float>(windowSize_.x);

	float y = -(static_cast<float>(Text2D::getTextHeight_()) * (row + 1.0f));
	if (row >= 0.0f) y += static_cast<float>(windowSize_.y);

	return vec2(x, y);
}
