#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "model.h"

struct CollisionNode {
	int x, z;
	CollisionNode *nw = nullptr;
	CollisionNode *ne = nullptr;
	CollisionNode *sw = nullptr;
	CollisionNode *se = nullptr;
};

enum Difficulty { EASY, NORMAL, HARD };

class Control {
public:
	Control(GLFWwindow *&, std::vector<std::vector<bool>>, std::vector<std::vector<glm::vec3>>, std::vector<std::vector<bool>>, std::vector<std::vector<glm::vec3>>, int, std::vector<glm::vec3>);
	~Control();
	glm::vec3 getPosition();
	void setStartPosition();
	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();
	bool computeMatrices(std::vector<Model> &, std::vector<std::vector<bool>> &, std::vector<std::vector<glm::vec3>>, Difficulty&);
	bool collisionExist(Model);
	bool collisionExistWithOtherModels(Model &, std::vector<Model>&, int);
	int calculatePossibleDirections(const Model &, bool &, bool &, bool &, bool &);
	int calculateShortest(Model, Model, int, std::vector<int> &, std::vector<int> &);
	bool started;
	int monsterCount;
	Difficulty difficulty;

private:
	CollisionNode root;
	GLFWwindow *window;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 right;
	glm::vec3 up;
	float fieldOfView;
	std::vector<std::vector<bool>> floorExistence;
	std::vector<std::vector<glm::vec3>> floorCoordinates;
	std::vector<std::vector<bool>> wallExistence;
	std::vector<std::vector<glm::vec3>> wallCoordinates;
	std::vector<int> verticalStepCounter;
	std::vector<int> horizontalStepCounter;
	int pacmanStep;
	int monsterStep;

};