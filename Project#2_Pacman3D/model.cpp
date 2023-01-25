#include "Model.h"
#include <iostream>
#include <fstream>
#include <glm/ext/matrix_transform.hpp>

Model::Model(std::string filepath, std::string filename) {
	lookingTop = true;
	lookingBottom = false;
	lookingLeft = false;
	lookingRight = false;
	loadObjectFile(filepath, filename);
}

Model::~Model() {}

/////////////////////////////////////////////////
// LOAD OBJECT FILE OF THE MODELS ~ STARTING ////
/////////////////////////////////////////////////

void Model::loadObjectFile(std::string filepath, std::string filename) {
	float x_min = std::numeric_limits<float>::max();
	float x_max = std::numeric_limits<float>::min();
	float z_min = std::numeric_limits<float>::max();
	float z_max = std::numeric_limits<float>::min();
	float x_total = 0.0f, z_total = 0.0f;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> textures;
	std::vector<glm::vec3> normals;
	std::ifstream file(filepath + "/" + filename);
	if (file) {
		char currentMtlName[100];
		std::string line;
		while (getline(file, line)) {
			if (startWith(line, "mtllib")) {
				char mtlFileName[100];
				(void)sscanf_s(line.c_str(), "mtllib %s", mtlFileName, sizeof(mtlFileName));
				loadMaterialFile(filepath + "/" + mtlFileName);
			}
			if (startWith(line, "v ")) {
				glm::vec3 pos;
				sscanf_s(line.c_str(), "v %f %f %f", &pos.x, &pos.y, &pos.z);
				x_total += pos.x; z_total += pos.z;
				if (pos.x > x_max) { x_max = pos.x; }
				if (pos.x < x_min) { x_min = pos.x; }
				if (pos.z > z_max) { z_max = pos.z; }
				if (pos.z < z_min) { z_min = pos.z; }		
				vertices.push_back(pos);
			}
			if (startWith(line, "vt ")) {
				glm::vec2 t;
				sscanf_s(line.c_str(), "vt %f %f", &t.x, &t.y);
				textures.push_back(t);
			}
			if (startWith(line, "vn ")) {
				glm::vec3 n;
				sscanf_s(line.c_str(), "vn %f %f %f", &n.x, &n.y, &n.z);
				normals.push_back(n);
			}
			if (startWith(line, "usemtl")) {
				(void)sscanf_s(line.c_str(), "usemtl %s", currentMtlName, sizeof(currentMtlName));
			}
			if (startWith(line, "f ")) {
				int v1, v2, v3, v4 = -1;
				int t1, t2, t3, t4 = -1;
				int n1, n2, n3, n4 = -1;
				(void)sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3, &v4, &t4, &n4);
				if (v4 == -1 && t4 == - 1 && n4 == -1) {
					addVertexData(v1, t1, n1, currentMtlName, vertices, textures, normals);
					addVertexData(v2, t2, n2, currentMtlName, vertices, textures, normals);
					addVertexData(v3, t3, n3, currentMtlName, vertices, textures, normals);
				} else {
					addVertexData(v1, t1, n1, currentMtlName, vertices, textures, normals);
					addVertexData(v2, t2, n2, currentMtlName, vertices, textures, normals);
					addVertexData(v3, t3, n3, currentMtlName, vertices, textures, normals);
					addVertexData(v3, t3, n3, currentMtlName, vertices, textures, normals);
					addVertexData(v4, t4, n4, currentMtlName, vertices, textures, normals);
					addVertexData(v1, t1, n1, currentMtlName, vertices, textures, normals);
				}
			}
		}
		x_center = x_total / vertices.size();
		z_center = z_total / vertices.size();
		radius = (x_max - x_min) / 2;
		currentDecision = NONE;
	}	else {
		std::cout << "Object file loading failed." << std::endl;
	}
}

//---------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////
// ADD MODEL DATA / Vertex / Texture / Color / Normal ~ STARTING ////
/////////////////////////////////////////////////////////////////////

void Model::addVertexData(std::vector<glm::vec3> &vertexData) {
	for (unsigned i = 0; i < mVertexData.size(); i++) {
		vertexData.push_back(mVertexData[i]);
	}
}

void Model::addTextureData(std::vector<glm::vec2> &textureData) {
	for (unsigned i = 0; i < mTextureData.size(); i++) {
		textureData.push_back(mTextureData[i]);
	}
}

void Model::addColorData(std::vector<glm::vec3> &colorData) {
	for (unsigned i = 0; i < mColorData.size(); i++) {
		colorData.push_back(mColorData[i]);
	}
}

void Model::addNormalData(std::vector<glm::vec3> &normalData) {
	for (unsigned i = 0; i < mNormalData.size(); i++) {
		normalData.push_back(mNormalData[i]);
	}
}

//---------------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// REMOVE MODEL DATA / Vertex / Texture / Color / Normal ~ STARTING //
//////////////////////////////////////////////////////////////////////

void Model::removeVertexData(std::vector<glm::vec3> &vertexData) {
	for (unsigned i = 0; i < mVertexData.size(); i++) {
		vertexData.pop_back();
	}
}

void Model::removeTextureData(std::vector<glm::vec2> &textureData) {
	for (unsigned i = 0; i < mTextureData.size(); i++) {
		textureData.pop_back();
	}
}

void Model::removeColorData(std::vector<glm::vec3> &colorData) {
	for (unsigned i = 0; i < mColorData.size(); i++) {
		colorData.pop_back();
	}
}

void Model::removeNormalData(std::vector<glm::vec3> &normalData) {
	for (unsigned i = 0; i < mNormalData.size(); i++) {
		normalData.pop_back();
	}
}

//---------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////
// ROTATE / TRANSLATE / TELEPORT / SCALE / SET SIZE MODELS //
/////////////////////////////////////////////////////////////

void Model::rotateQuarterCycle() {
	glm::mat4 transformMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	for (unsigned i = 0; i < mVertexData.size(); i++) {
		glm::vec4 point(mVertexData[i].x - x_center, mVertexData[i].y, mVertexData[i].z - z_center, 1.0f);
		point = transformMatrix * point;
		point.x += x_center; point.z += z_center;
		mVertexData[i] = point;
	}
}

void Model::translate(glm::vec3 translation) {
	for (unsigned i = 0; i < mVertexData.size(); i++) {
		mVertexData[i] += translation;
	}
	x_center += translation.x;
	z_center += translation.z;
}

void Model::scale(float factor) {
	glm::mat4 transformMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(factor, factor, factor));
	for (unsigned i = 0; i < mVertexData.size(); i++) {
		glm::vec4 point(mVertexData[i].x - x_center, mVertexData[i].y, mVertexData[i].z - z_center, 1.0f);
		point = transformMatrix * point;
		point.x += x_center; point.z += z_center;
		mVertexData[i] = point;
	}
	radius *= factor;
}

void Model::teleport(glm::vec3 point) {
	glm::vec3 translation = point - glm::vec3(x_center, 0.5f, z_center);
	translate(translation);
}

void Model::setSize(float size) {
	float scalingFactor = size / radius;
	scale(scalingFactor);
}

glm::vec3 Model::getCenter() { return glm::vec3(x_center, 0, z_center); }

//---------------------------------------------------------------------------------------------------

//////////////////////////////////////////////
// LOOK DIRECTIONS OF THE MODELS ~ STARTING //
//////////////////////////////////////////////

void Model::lookTop() {
	if (lookingBottom) { rotateQuarterCycle(); rotateQuarterCycle(); }
	else if (lookingLeft) { rotateQuarterCycle(); }
	else if (lookingRight) { rotateQuarterCycle(); rotateQuarterCycle(); rotateQuarterCycle(); }
	lookingTop = true; 
	lookingBottom = false; 
	lookingLeft = false; 
	lookingRight = false; 
}

void Model::lookBottom() { 
	if (lookingTop) { rotateQuarterCycle(); rotateQuarterCycle(); }
	else if (lookingLeft) { rotateQuarterCycle(); rotateQuarterCycle(); rotateQuarterCycle(); }
	else if (lookingRight) { rotateQuarterCycle(); }
	lookingTop = false; 
	lookingBottom = true; 
	lookingLeft = false; 
	lookingRight = false;
}

void Model::lookLeft() { 
	if (lookingTop) { rotateQuarterCycle(); rotateQuarterCycle(); rotateQuarterCycle(); }
	else if (lookingBottom) { rotateQuarterCycle(); }
	else if (lookingRight) { rotateQuarterCycle(); rotateQuarterCycle(); }
	lookingTop = false; 
	lookingBottom = false; 
	lookingLeft = true; 
	lookingRight = false; 
}

void Model::lookRight() {
	if (lookingTop) { rotateQuarterCycle(); }
	else if (lookingBottom) { rotateQuarterCycle(); rotateQuarterCycle(); rotateQuarterCycle(); }
	else if (lookingLeft) { rotateQuarterCycle(); rotateQuarterCycle(); }
	lookingTop = false; 
	lookingBottom = false; 
	lookingLeft = false; 
	lookingRight = true; 
}

//---------------------------------------------------------------------------------------------------

/////////////////////////////////////////////////
// LOAD MATERIAL FILE OF THE MODELS ~ STARTING //
/////////////////////////////////////////////////

void Model::loadMaterialFile(std::string filename) {
	std::ifstream file(filename);
	if (file) {
		std::string line;
		while (getline(file, line)) {
			char mtlName[100];
			if (startWith(line, "newmtl")) {
				(void)sscanf_s(line.c_str(), "newmtl %s", mtlName, sizeof(mtlName));
				mMaterialMap[mtlName] = glm::vec3();
			}
			if (startWith(line, "Kd")) {
				glm::vec3 &color = mMaterialMap[mtlName];
				sscanf_s(line.c_str(), "Kd %f %f %f", &color.r, &color.g, &color.b);
			}
		}
	}	else {
		std::cout << "Material file loading failed." << std::endl;
	}
}

bool Model::startWith(std::string &line, std::string text) {
	size_t textLen = text.length();
	if (line.size() < textLen) {
		return false;
	}
	for (size_t i = 0; i < textLen; i++) {
		if (line[i] == text[i]) {
			continue;
		}	else {
			return false;
		}
	}
	return true;
}

//---------------------------------------------------------------------------------------------------

void Model::addVertexData(int vIdx, int tIdx, int nIdx, const char *mtl, std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &textures, std::vector<glm::vec3> &normals) {
	glm::vec3 point = vertices[vIdx - 1];
	glm::vec2 texture = textures[tIdx - 1];
	glm::vec3 color = mMaterialMap[mtl];
	glm::vec3 normal = normals[nIdx - 1];
	mVertexData.push_back(point);
	mTextureData.push_back(texture);
	mColorData.push_back(color);
	mNormalData.push_back(normal);
}