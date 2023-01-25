#pragma once

#include <vector>
#include <string>
#include <map>
#include <glm/glm.hpp>

enum Direction { LEFT, RIGHT, TOP, BOTTOM, NONE };

class Model {
public:
	Model(std::string, std::string);
	~Model();
	void addVertexData(std::vector<glm::vec3> &);
	void addTextureData(std::vector<glm::vec2> &);
	void addColorData(std::vector<glm::vec3> &);
	void addNormalData(std::vector<glm::vec3> &);
	void removeVertexData(std::vector<glm::vec3> &);
	void removeTextureData(std::vector<glm::vec2> &);
	void removeColorData(std::vector<glm::vec3> &);
	void removeNormalData(std::vector<glm::vec3> &);
	void rotateQuarterCycle();
	void translate(glm::vec3);
	void scale(float);
	void teleport(glm::vec3);
	void setSize(float);
	glm::vec3 getCenter();
	void lookTop();
	void lookBottom();
	void lookLeft();
	void lookRight();
	std::vector<glm::vec3> mVertexData;
	std::vector<glm::vec2> mTextureData;
	std::vector<glm::vec3> mColorData;
	std::vector<glm::vec3> mNormalData;
	float x_center, z_center;
	float radius;
	Direction comingFrom;
	Direction currentDecision;
	bool lookingTop, lookingBottom, lookingLeft, lookingRight;

private:
	void loadObjectFile(std::string, std::string);
	void loadMaterialFile(std::string);
	bool startWith(std::string &, std::string);
	void addVertexData(int, int, int, const char *, std::vector<glm::vec3> &, std::vector<glm::vec2> &, std::vector<glm::vec3> &);
	std::map<std::string, glm::vec3> mMaterialMap;

};