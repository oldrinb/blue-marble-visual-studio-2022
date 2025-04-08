/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 21, 2021
 **/

#include "TriangleList2D.h"



TriangleList2D::TriangleList2D(unsigned int nVertices) : Triangle2D() {
	if (nVertices > 0u && nVertices % 3u == 0u) nVertices_ = static_cast<GLsizei>(nVertices);
	else throw runtime_error("TriangleList2D|Invalid number of vertices value.");

	//cout << "TriangleList2D created." << endl;
}



TriangleList2D::~TriangleList2D() {
	//cout << "TriangleList2D deleted." << endl;
}



void TriangleList2D::setNumVertices(unsigned int nVertices) {	
	if (nVertices > 0u && nVertices % 3u == 0u) {
		nVertices_ = static_cast<GLsizei>(nVertices);

		Triangle2D::clearVerticesData_();
	}
	else throw runtime_error("TriangleList2D.setNumVertices|Invalid number of vertices value.");	
}
