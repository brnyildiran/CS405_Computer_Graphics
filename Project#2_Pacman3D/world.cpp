#include "world.h"
#include <fstream>
#include <sstream>
#include <iostream>

World::World(std::string filename) {
	std::ifstream inputFileStream(filename);
	std::string firstLine;
	getline(inputFileStream, firstLine);
	std::istringstream firstLineParser(firstLine);
	firstLineParser >> xNumCells >> zNumCells >> range;
	float xMin = (xNumCells - 1) / -2.0;
	float xMax = (xNumCells - 1) / 2.0;
	float zMin = (zNumCells - 1) / -2.0;
	float zMax = (zNumCells - 1) / 2.0;
	wallLocations = std::vector<std::vector<glm::vec3>>(xNumCells, std::vector<glm::vec3>(zNumCells, glm::vec3(0, 0, 0)));
	bulbLocations = std::vector<std::vector<glm::vec3>>(xNumCells / 2 - 1, std::vector<glm::vec3>(zNumCells / 2 - 1, glm::vec3(0, 0, 0)));
	wallExistence = std::vector<std::vector<bool>>(xNumCells, std::vector<bool>(zNumCells, false));
	bulbExistence = std::vector<std::vector<bool>>(xNumCells / 2 - 1, std::vector<bool>(zNumCells / 2 - 1, false));
	std::string eachLine;
	while (getline(inputFileStream, eachLine)) {
		std::string p;
		float x, y, z;
		std::istringstream eachLineParser(eachLine);
		eachLineParser >> p >> x >> y >> z;
		if (p == "f") {
			addFloor(x, y, z);
			if (int(abs(x) - 0.5f) % 2 == 0 && int(abs(z) - 0.5f) % 2 == 0) {
				int xIndex = (x - xMin + 1) / (range + 1) - 1;
				int zIndex = (z - zMin + 1) / (range + 1) - 1;
				if (x > 0 && z > 0) {
					bulbLocations[zNumCells / 2 - 2 - zIndex][xIndex] = glm::vec3(x - 0.5f, 0.225f, z - 0.5f);
					bulbExistence[zNumCells / 2 - 2 - zIndex][xIndex] = true;
				}
				else if (x > 0 && z < 0) {
					bulbLocations[zNumCells / 2 - 2 - zIndex][xIndex] = glm::vec3(x - 0.5f, 0.225f, z + 0.5f);
					bulbExistence[zNumCells / 2 - 2 - zIndex][xIndex] = true;
				}
				else if (x < 0 && z > 0) {
					bulbLocations[zNumCells / 2 - 2 - zIndex][xIndex] = glm::vec3(x + 0.5f, 0.225f, z - 0.5f);
					bulbExistence[zNumCells / 2 - 2 - zIndex][xIndex] = true;
				}
				else if (x < 0 && z < 0) {
					bulbLocations[zNumCells / 2 - 2 - zIndex][xIndex] = glm::vec3(x + 0.5f, 0.225f, z + 0.5f);
					bulbExistence[zNumCells / 2 - 2 - zIndex][xIndex] = true;
				}
			}
		}
		else if (p == "w") {
			int xIndex = (x - xMin + 1) / range - 1;
			int zIndex = (z - zMin + 1) / range - 1;
			wallLocations[zNumCells - 1 - zIndex][xIndex] = glm::vec3(x, y, z);
			wallExistence[zNumCells - 1 - zIndex][xIndex] = true;
		}
	}
	for (int xIndex = 0; xIndex < xNumCells; xIndex++) {
		for (int zIndex = 0; zIndex < zNumCells; zIndex++) {
			if (wallExistence[xIndex][zIndex]) {
				glm::vec3 currentWallCoordinates(wallLocations[xIndex][zIndex]);
				addWall(currentWallCoordinates.x, currentWallCoordinates.y, currentWallCoordinates.z, xIndex, zIndex);
			}
		}
	}
	floorExistence = bulbExistence;
	floorLocations = bulbLocations;
	inputFileStream.close();
}

World::~World() {}

// Adding vertices, colors and normals to the floor
void World::addFloor(float x, float y, float z) {

	// Vertices & normals for the first triangle
	vertices.push_back(glm::vec3(x - 0.5f, y, z - 0.5f));
	vertices.push_back(glm::vec3(x + 0.5f, y, z - 0.5f));
	vertices.push_back(glm::vec3(x + 0.5f, y, z + 0.5f));

	glm::vec3 crossProduct = glm::normalize(glm::cross(
		glm::vec3(x + 0.5f, y, z - 0.5f) - glm::vec3(x - 0.5f, y, z - 0.5f),
		glm::vec3(x + 0.5f, y, z + 0.5f) - glm::vec3(x - 0.5f, y, z - 0.5f)
	));

	normals.push_back(crossProduct);
	normals.push_back(crossProduct);
	normals.push_back(crossProduct);

	// Vertices & normals for the second triangle
	vertices.push_back(glm::vec3(x + 0.5f, y, z + 0.5f));
	vertices.push_back(glm::vec3(x - 0.5f, y, z + 0.5f));
	vertices.push_back(glm::vec3(x - 0.5f, y, z - 0.5f));

	crossProduct = glm::normalize(glm::cross(
		glm::vec3(x - 0.5f, y, z + 0.5f) - glm::vec3(x + 0.5f, y, z + 0.5f),
		glm::vec3(x - 0.5f, y, z - 0.5f) - glm::vec3(x + 0.5f, y, z + 0.5f)
	));

	normals.push_back(crossProduct);
	normals.push_back(crossProduct);
	normals.push_back(crossProduct);


	// Colors for both triangles
	for (int _ = 0; _ < 6; _++) {
		colors.push_back(glm::vec3(0.2f, 0.2f, 0.6f));
	}
}

void World::addWall(float x, float y, float z, int xIndex, int zIndex) {
	// Vertices & normals for the first triangle of upward facing wall
	vertices.push_back(glm::vec3(x + 0.5f, y + 1.0f, z + 0.5f));
	vertices.push_back(glm::vec3(x + 0.5f, y + 1.0f, z - 0.5f));
	vertices.push_back(glm::vec3(x - 0.5f, y + 1.0f, z - 0.5f));

	glm::vec3 crossProduct = glm::normalize(glm::cross(
		glm::vec3(x + 0.5f, y + 1.0f, z - 0.5f) - glm::vec3(x + 0.5f, y + 1.0f, z + 0.5f),
		glm::vec3(x - 0.5f, y + 1.0f, z - 0.5f) - glm::vec3(x + 0.5f, y + 1.0f, z + 0.5f)
	));

	normals.push_back(crossProduct);
	normals.push_back(crossProduct);
	normals.push_back(crossProduct);

	// Vertices & normals for the second triangle of upward facing wall
	vertices.push_back(glm::vec3(x - 0.5f, y + 1.0f, z - 0.5f));
	vertices.push_back(glm::vec3(x - 0.5f, y + 1.0f, z + 0.5f));
	vertices.push_back(glm::vec3(x + 0.5f, y + 1.0f, z + 0.5f));

	crossProduct = glm::normalize(glm::cross(
		glm::vec3(x - 0.5f, y + 1.0f, z + 0.5f) - glm::vec3(x - 0.5f, y + 1.0f, z - 0.5f),
		glm::vec3(x + 0.5f, y + 1.0f, z + 0.5f) - glm::vec3(x - 0.5f, y + 1.0f, z - 0.5f)
	));

	normals.push_back(crossProduct);
	normals.push_back(crossProduct);
	normals.push_back(crossProduct);

	// Colors for both triangles of upward facing wall
	for (int _ = 0; _ < 6; _++) {
		colors.push_back(glm::vec3(0.4f, 0.4f, 1.0f));
	}

	if (zIndex == 0 || (zIndex != 0 && !wallExistence[xIndex][zIndex - 1])) {
		// Vertices & normals for the first triangle of left facing wall
		vertices.push_back(glm::vec3(x - 0.5f, y + 1.0f, z + 0.5f));
		vertices.push_back(glm::vec3(x - 0.5f, y + 1.0f, z - 0.5f));
		vertices.push_back(glm::vec3(x - 0.5f, y, z - 0.5f));

		crossProduct = glm::normalize(glm::cross(
			glm::vec3(x - 0.5f, y + 1.0f, z - 0.5f) - glm::vec3(x - 0.5f, y + 1.0f, z + 0.5f),
			glm::vec3(x - 0.5f, y, z - 0.5f) - glm::vec3(x - 0.5f, y + 1.0f, z + 0.5f)
		));

		normals.push_back(crossProduct);
		normals.push_back(crossProduct);
		normals.push_back(crossProduct);

		// Vertices & normals for the second triangle of left facing wall
		vertices.push_back(glm::vec3(x - 0.5f, y, z - 0.5f));
		vertices.push_back(glm::vec3(x - 0.5f, y, z + 0.5f));
		vertices.push_back(glm::vec3(x - 0.5f, y + 1.0f, z + 0.5f));

		crossProduct = glm::normalize(glm::cross(
			glm::vec3(x - 0.5f, y, z + 0.5f) - glm::vec3(x - 0.5f, y, z - 0.5f),
			glm::vec3(x - 0.5f, y + 1.0f, z + 0.5f) - glm::vec3(x - 0.5f, y, z - 0.5f)
		));

		normals.push_back(crossProduct);
		normals.push_back(crossProduct);
		normals.push_back(crossProduct);

		// Colors for both triangles of left facing wall
		for (int _ = 0; _ < 6; _++) {
			colors.push_back(glm::vec3(0.4f, 0.4f, 1.0f));
		}
	}
	if (zIndex == zNumCells - 1 || (zIndex != zNumCells - 1 && !wallExistence[xIndex][zIndex + 1])) {
		// Vertices & normals for the first triangle of right facing wall
		vertices.push_back(glm::vec3(x + 0.5f, y, z - 0.5f));
		vertices.push_back(glm::vec3(x + 0.5f, y, z + 0.5f));
		vertices.push_back(glm::vec3(x + 0.5f, y + 1.0f, z + 0.5f));

		crossProduct = glm::normalize(glm::cross(
			glm::vec3(x + 0.5f, y, z + 0.5f) - glm::vec3(x + 0.5f, y, z - 0.5f),
			glm::vec3(x + 0.5f, y + 1.0f, z + 0.5f) - glm::vec3(x + 0.5f, y, z - 0.5f)
		));

		normals.push_back(crossProduct);
		normals.push_back(crossProduct);
		normals.push_back(crossProduct);

		// Vertices & normals for the second triangle of right facing wall
		vertices.push_back(glm::vec3(x + 0.5f, y + 1.0f, z + 0.5f));
		vertices.push_back(glm::vec3(x + 0.5f, y + 1.0f, z - 0.5f));
		vertices.push_back(glm::vec3(x + 0.5f, y, z - 0.5f));

		crossProduct = glm::normalize(glm::cross(
			glm::vec3(x + 0.5f, y + 1.0f, z - 0.5f) - glm::vec3(x + 0.5f, y + 1.0f, z + 0.5f),
			glm::vec3(x + 0.5f, y, z - 0.5f) - glm::vec3(x + 0.5f, y + 1.0f, z + 0.5f)
		));

		normals.push_back(crossProduct);
		normals.push_back(crossProduct);
		normals.push_back(crossProduct);

		// Colors for both triangles of right facing wall
		for (int _ = 0; _ < 6; _++) {
			colors.push_back(glm::vec3(0.4f, 0.4f, 1.0f));
		}
	}
	if (xIndex == 0 || (xIndex != 0 && !wallExistence[xIndex - 1][zIndex])) {
		// Vertices & normals for the first triangle of backward facing wall
		vertices.push_back(glm::vec3(x + 0.5f, y, z + 0.5f)); // 5
		vertices.push_back(glm::vec3(x + 0.5f, y + 1.0f, z + 0.5f)); // 3
		vertices.push_back(glm::vec3(x + 0.5f, y + 1.0f, z - 0.5f)); // 4

		crossProduct = glm::normalize(glm::cross(
			glm::vec3(x + 0.5f, y + 1.0f, z + 0.5f) - glm::vec3(x + 0.5f, y, z + 0.5f),
			glm::vec3(x + 0.5f, y + 1.0f, z - 0.5f) - glm::vec3(x + 0.5f, y, z + 0.5f)
		));

		normals.push_back(crossProduct);
		normals.push_back(crossProduct);
		normals.push_back(crossProduct);

		// Vertices & normals for the second triangle of backward facing wall
		vertices.push_back(glm::vec3(x + 0.5f, y + 1.0f, z - 0.5f)); // 2
		vertices.push_back(glm::vec3(x - 0.5f, y, z + 0.5f)); // 6
		vertices.push_back(glm::vec3(x + 0.5f, y, z + 0.5f)); // 1

		crossProduct = glm::normalize(glm::cross(
			glm::vec3(x - 0.5f, y, z + 0.5f) - glm::vec3(x + 0.5f, y + 1.0f, z - 0.5f),
			glm::vec3(x + 0.5f, y, z + 0.5f) - glm::vec3(x + 0.5f, y + 1.0f, z - 0.5f)
		));

		normals.push_back(crossProduct);
		normals.push_back(crossProduct);
		normals.push_back(crossProduct);

		// Colors for both triangles of backward facing wall
		for (int _ = 0; _ < 6; _++) {
			colors.push_back(glm::vec3(0.4f, 0.4f, 1.0f));
		}
	}
	if (xIndex == xNumCells - 1 || (xIndex != xNumCells - 1 && !wallExistence[xIndex + 1][zIndex])) {
		// Vertices & normals for the first triangle of forward facing wall
		vertices.push_back(glm::vec3(x - 0.5f, y, z - 0.5f)); // 3
		vertices.push_back(glm::vec3(x - 0.5f, y + 1.0f, z - 0.5f)); // 2
		vertices.push_back(glm::vec3(x + 0.5f, y + 1.0f, z - 0.5f)); // 1

		crossProduct = glm::normalize(glm::cross(
			glm::vec3(x - 0.5f, y + 1.0f, z - 0.5f) - glm::vec3(x - 0.5f, y, z - 0.5f),
			glm::vec3(x + 0.5f, y + 1.0f, z - 0.5f) - glm::vec3(x - 0.5f, y, z - 0.5f)
		));

		normals.push_back(crossProduct);
		normals.push_back(crossProduct);
		normals.push_back(crossProduct);

		// Vertices & normals for the second triangle of forward facing wall
		vertices.push_back(glm::vec3(x + 0.5f, y + 1.0f, z - 0.5f)); // 6
		vertices.push_back(glm::vec3(x + 0.5f, y, z - 0.5f)); // 5
		vertices.push_back(glm::vec3(x - 0.5f, y, z - 0.5f)); // 4

		crossProduct = glm::normalize(glm::cross(
			glm::vec3(x + 0.5f, y, z - 0.5f) - glm::vec3(x + 0.5f, y + 1.0f, z - 0.5f),
			glm::vec3(x - 0.5f, y, z - 0.5f) - glm::vec3(x + 0.5f, y + 1.0f, z - 0.5f)
		));

		normals.push_back(crossProduct);
		normals.push_back(crossProduct);
		normals.push_back(crossProduct);

		// Colors for both triangles of forward facing wall
		for (int _ = 0; _ < 6; _++) {
			colors.push_back(glm::vec3(0.4f, 0.4f, 1.0f));
		}
	}
}