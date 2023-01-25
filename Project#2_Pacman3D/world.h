#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>

class World {
public:
	World(std::string);
	~World();
	std::vector<std::vector<bool>> wallExistence;
	std::vector<std::vector<bool>> bulbExistence;
	std::vector<std::vector<bool>> floorExistence;
	std::vector<std::vector<glm::vec3>> wallLocations;
	std::vector<std::vector<glm::vec3>> bulbLocations;
	std::vector<std::vector<glm::vec3>> floorLocations;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> colors;
	std::vector<glm::vec3> normals;

private:
	int xNumCells;
	int zNumCells;
	int range;
	void addFloor(float, float, float);
	void addWall(float, float, float, int, int);
	float xMin, xMax, zMin, zMax;

};