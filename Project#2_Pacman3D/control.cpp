#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "control.h"
#include <iostream>
#include <stack>
#include <queue>
#include <unordered_map>

Control::Control(GLFWwindow *&_window, std::vector<std::vector<bool>> _wallExistence, std::vector<std::vector<glm::vec3>> _wallCoordinates, std::vector<std::vector<bool>> _floorExistence, std::vector<std::vector<glm::vec3>> _floorCoordinates, int _monsterCount, std::vector<glm::vec3> wallsCoordinates) : window(_window), wallExistence(_wallExistence), wallCoordinates(_wallCoordinates), floorExistence(_floorExistence), floorCoordinates(_floorCoordinates), monsterCount(_monsterCount) {
	position = glm::vec3(8.0f, 25.0f, -20.0f);
	fieldOfView = 45.0f;
	direction = glm::vec3(0.0f, sin(5.45f), cos(5.45f));
	right = glm::vec3(-1.0f, 0.0f, 0.0f);
	up = glm::cross(right, direction);
	projectionMatrix = glm::perspective(glm::radians(fieldOfView), 16.0f / 9.0f, 0.1f, 100.0f);
	viewMatrix = glm::lookAt(position, position + direction, up);
	verticalStepCounter = std::vector<int>(monsterCount + 1, 0);
	horizontalStepCounter = std::vector<int>(monsterCount + 1, 0);
	started = false;
	difficulty = NORMAL;
	pacmanStep = 5;
	monsterStep = 25;
	
	// Add Walls to the Quad Tree
	root.x = wallsCoordinates[0].x;
	root.z = wallsCoordinates[0].z;
	for (unsigned i = 1; i < wallsCoordinates.size(); i++) {
		CollisionNode *curr = &root;
		while (true) {
			int x = wallsCoordinates[i].x;
			int z = wallsCoordinates[i].z;
			if (x < curr->x && z >= curr->z) {
				if (curr->nw == nullptr) {
					curr->nw = new CollisionNode();
					curr->nw->x = x;
					curr->nw->z = z;
					break;
				}	else {
					curr = curr->nw;
				}
			} else if (x >= curr->x && z >= curr->z) {
				if (curr->ne == nullptr) {
					curr->ne = new CollisionNode();
					curr->ne->x = x;
					curr->ne->z = z;
					break;
				}	else {
					curr = curr->ne;
				}
			}	else if (x < curr->x && z < curr->z) {
				if (curr->sw == nullptr) {
					curr->sw = new CollisionNode();
					curr->sw->x = x;
					curr->sw->z = z;
					break;
				}	else {
					curr = curr->sw;
				}
			}	else if (x >= curr->x && z < curr->z) {
				if (curr->se == nullptr) {
					curr->se = new CollisionNode();
					curr->se->x = x;
					curr->se->z = z;
					break;
				} else {
					curr = curr->se;
				}
			} else {
				break;
			}
		}
	}
}

Control::~Control() {}

glm::mat4 Control::getViewMatrix() { return viewMatrix; }

glm::vec3 Control::getPosition() { return position; }

void Control::setStartPosition() { position = glm::vec3(8.0f, 25.0f, -20.0f); }

glm::mat4 Control::getProjectionMatrix() { return projectionMatrix; }

//---------------------------------------------------------------------------------------------------

////////////////////////////////////
// COLLUSION DETECTION ~ STARTING //
////////////////////////////////////

bool Control::collisionExist(Model model) {
	glm::vec3 modelTargetPoint;
	if (model.lookingTop) { modelTargetPoint = glm::vec3(model.x_center, 0, model.z_center + 2); }
	else if (model.lookingBottom) { modelTargetPoint = glm::vec3(model.x_center, 0, model.z_center - 2); }
	else if (model.lookingRight) { modelTargetPoint = glm::vec3(model.x_center + 2, 0, model.z_center); }
	else if (model.lookingLeft) { modelTargetPoint = glm::vec3(model.x_center - 2, 0, model.z_center); }
	
	int modelTargetPointX = modelTargetPoint.x;
	int modelTargetPointZ = modelTargetPoint.z;

	CollisionNode *currentNode = &root;
	while (true) {
		if (currentNode == nullptr) {
			return false;
		}
		if (currentNode->x == modelTargetPointX && currentNode->z == modelTargetPointZ) {
			return true;
		}
		if (modelTargetPointX < currentNode->x && modelTargetPointZ >= currentNode->z) {
			currentNode = currentNode->nw;
		} else if (modelTargetPointX >= currentNode->x && modelTargetPointZ >= currentNode->z) {
			currentNode = currentNode->ne;
		} else if (modelTargetPointX < currentNode->x && modelTargetPointZ < currentNode->z) {
			currentNode = currentNode->sw;
		} else if (modelTargetPointX >= currentNode->x && modelTargetPointZ < currentNode->z) {
			currentNode = currentNode->se;
		}
	}
}

//---------------------------------------------------------------------------------------------------


bool Control::computeMatrices(std::vector<Model> &models, std::vector<std::vector<bool>> &bulbsExistence, std::vector<std::vector<glm::vec3>> bulbsCoordinates, Difficulty &currentDifficulty) {
	
	/////////////////////////////////////////////
	// START WHEN ANY OF THESE BUTTONS PRESSED //
	/////////////////////////////////////////////

	if (!started && (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || 
		glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)) {
		started = true;
		return false;
	}

	//////////////////////////
	// DIFFICULTY SELECTION //
	//////////////////////////

	if (!started) {
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
			difficulty = EASY;
			currentDifficulty = EASY;
			monsterStep = 30;
		}
		else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
			difficulty = NORMAL;
			currentDifficulty = NORMAL;
			monsterStep = 25;
		}
		else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
			difficulty = HARD;
			currentDifficulty = HARD;
			monsterStep = 20;
		}
		return false;
	}

	///////////////////////////////////////////////
	// CALCULATING PACMAN COORDINATES ~ STARTING //
	///////////////////////////////////////////////

	glm::vec3 pacmanCenter = models[0].getCenter();
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		horizontalStepCounter[0] = 0;
		verticalStepCounter[0]++;
		models[0].lookTop();
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		horizontalStepCounter[0] = 0;
		verticalStepCounter[0]--;
		models[0].lookBottom();
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		horizontalStepCounter[0]++;
		verticalStepCounter[0] = 0;
		models[0].lookRight();
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		horizontalStepCounter[0]--;
		verticalStepCounter[0] = 0;
		models[0].lookLeft();
	}

	if (verticalStepCounter[0] == pacmanStep) {
		verticalStepCounter[0] = 0;
		horizontalStepCounter[0] = 0;
		if (!collisionExist(models[0])) {
			position += glm::vec3(0.0f, 0.0f, 2.0f);
			models[0].teleport(pacmanCenter + glm::vec3(0.0f, 0.5f, 2.0f));
		}
	}
	if (verticalStepCounter[0] == -pacmanStep) {
		verticalStepCounter[0] = 0;
		horizontalStepCounter[0] = 0;
		if (!collisionExist(models[0])) {
			position += glm::vec3(0.0f, 0.0f, -2.0f);
			models[0].teleport(pacmanCenter + glm::vec3(0.0f, 0.5f, -2.0f));
		}
	}
	if (horizontalStepCounter[0] == pacmanStep) {
		verticalStepCounter[0] = 0;
		horizontalStepCounter[0] = 0;
		if (!collisionExist(models[0])) {
			if (models[0].x_center == 18) {
				position += glm::vec3(36.0f, 0.0f, 0.0f);
				models[0].teleport(pacmanCenter + glm::vec3(-36.0f, 0.5f, 0.0f));
			} else {
				position += glm::vec3(-2.0f, 0.0f, 0.0f);
				models[0].teleport(pacmanCenter + glm::vec3(2.0f, 0.5f, 0.0f));
			}
		}
	}
	if (horizontalStepCounter[0] == -pacmanStep) {
		verticalStepCounter[0] = 0;
		horizontalStepCounter[0] = 0;
		if (!collisionExist(models[0])) {
			if (models[0].x_center == -18) {
				position += glm::vec3(-36.0f, 0.0f, 0.0f);
				models[0].teleport(pacmanCenter + glm::vec3(36.0f, 0.5f, 0.0f));
			} else {
				position += glm::vec3(2.0f, 0.0f, 0.0f);
				models[0].teleport(pacmanCenter + glm::vec3(-2.0f, 0.5f, 0.0f));
			}
		}
	}

	// Collecting Bulbs
	for (unsigned i = 0; i < bulbsCoordinates.size(); i++) {
		for (unsigned j = 0; j < bulbsCoordinates[i].size(); j++) {
			if (bulbsCoordinates[i][j].x == models[0].x_center && bulbsCoordinates[i][j].z == models[0].z_center) {
				bulbsExistence[i][j] = false;
			}
		}
	}
	
	//---------------------------------------------------------------------------------------------------

	/////////////////////////////////////////////////////
	// CALCULATING MONSTER COORDINATES / AI ~ STARTING //
	/////////////////////////////////////////////////////

	std::vector<int> monsterDistancesToPacman(monsterCount + 1);
	std::vector<int> monsterClosestNextX(monsterCount + 1);
	std::vector<int> monsterClosestNextZ(monsterCount + 1);

	// Calculating Djikstra shortest path
	for (unsigned i = 1; i < monsterCount + 1; i++) {
		monsterDistancesToPacman[i] = calculateShortest(models[0], models[i], i, monsterClosestNextX, monsterClosestNextZ);
	}

	// How many monster will stalk based on difficulty
	int stalkingMonstersCount = 0;
	if (difficulty == EASY || difficulty == NORMAL) { stalkingMonstersCount = 1; }
	else if (difficulty == HARD) { stalkingMonstersCount = 2; }
	
	// Sorting the distance of monsters to the pacman [smaller --> larger]
	std::unordered_map<int, bool> stalkingMonstersMap;
	std::vector<int> monsterDistancesToPacmanSorted(monsterDistancesToPacman);
	std::sort(monsterDistancesToPacmanSorted.begin(), monsterDistancesToPacmanSorted.end());
	monsterDistancesToPacmanSorted[0] = std::numeric_limits<int>::max();

	while (monsterDistancesToPacmanSorted.size() > stalkingMonstersCount + 1) {
		monsterDistancesToPacmanSorted.pop_back();
	}

	int followed = 0;

	// Closest monster(s) should stalk
	for (unsigned i = 1; i < monsterDistancesToPacman.size(); i++) {
		stalkingMonstersMap[i] = false;

		// [Easy / Medium --> 1] [Hard --> 2] 
		if (followed < stalkingMonstersCount) {
			for (unsigned j = 1; j < monsterDistancesToPacmanSorted.size(); j++) {
				if (monsterDistancesToPacman[i] == monsterDistancesToPacmanSorted[j]) {
					followed++;
					stalkingMonstersMap[i] = true;
					break;
				}
			}
		}
	}

	for (unsigned i = 1; i < monsterCount + 1; i++) {
		// Planned movement --> Djikstra
		if (stalkingMonstersMap[i]) {
			int source_x = (-1) * models[i].z_center / 2 + 9;
			int source_z = models[i].x_center / 2 + 9;
			if (monsterClosestNextX[i] < source_x) {
				verticalStepCounter[i] += 1;
			}
			else if (monsterClosestNextX[i] > source_x) {
				verticalStepCounter[i] -= 1;
			}
			else if (monsterClosestNextZ[i] < source_z) {
				horizontalStepCounter[i] -= 1;
			}
			else if (monsterClosestNextZ[i] > source_z) {
				horizontalStepCounter[i] += 1;
			}
		}
		// Random movement 
		else {
			bool leftAllowed = false;
			bool rightAllowed = false;
			bool topAllowed = false;
			bool bottomAllowed = false;
			int totalMoves = calculatePossibleDirections(models[i], leftAllowed, rightAllowed, topAllowed, bottomAllowed);
			if (totalMoves == 1) {
				if (leftAllowed) {
					horizontalStepCounter[i] -= 1;
				}
				else if (rightAllowed) {
					horizontalStepCounter[i] += 1;
				}
				else if (bottomAllowed) {
					verticalStepCounter[i] -= 1;
				}
				else if (topAllowed) {
					verticalStepCounter[i] += 1;
				}
			}
			else {
				// Do not enter to the monster field
				if (models[i].x_center == 0 && models[i].z_center == 0) {
					models[i].currentDecision = TOP;
					continue;
				}
				std::vector<char> possibleDirections;
				if (leftAllowed) { possibleDirections.push_back('l'); }
				if (rightAllowed) { possibleDirections.push_back('r'); }
				if (bottomAllowed) { possibleDirections.push_back('b'); }
				if (topAllowed) { possibleDirections.push_back('t'); }
				int r = rand() % possibleDirections.size();
				if (possibleDirections[r] == 'l') {
					models[i].currentDecision = LEFT;
				}
				else if (possibleDirections[r] == 'r') {
					models[i].currentDecision = RIGHT;
				}
				else if (possibleDirections[r] == 'b') {
					models[i].currentDecision = BOTTOM;
				}
				else if (possibleDirections[r] == 't') {
					models[i].currentDecision = TOP;
				}
			}
		}
	}

	// Whether the monsters passed its monsterStep --> make the movement decided above
	for (unsigned i = 1; i < monsterCount + 1; i++) {
		if (verticalStepCounter[i] >= monsterStep) {
			models[i].lookTop();
			if (!collisionExist(models[i])) {
				horizontalStepCounter[i] = 0; verticalStepCounter[i] = 0;
				models[i].currentDecision = NONE;
				if (!collisionExistWithOtherModels(models[i], models, i))	{
					models[i].teleport(models[i].getCenter() + glm::vec3(0.0f, 0.5f, 2.0f));
					models[i].comingFrom = BOTTOM;
				} else {
					models[i].comingFrom = NONE;
				}
			}
		}
		else if (verticalStepCounter[i] <= -monsterStep) {
			models[i].lookBottom();
			if (!collisionExist(models[i])) {
				horizontalStepCounter[i] = 0; verticalStepCounter[i] = 0;
				models[i].currentDecision = NONE;
				if (!collisionExistWithOtherModels(models[i], models, i)) {
					models[i].teleport(models[i].getCenter() + glm::vec3(0.0f, 0.5f, -2.0f));
					models[i].comingFrom = TOP;
				} else {
					models[i].comingFrom = NONE;
				}
			}
		}
		else if (horizontalStepCounter[i] >= monsterStep) {
			models[i].lookRight();
			if (!collisionExist(models[i])) {
				horizontalStepCounter[i] = 0; verticalStepCounter[i] = 0;
				models[i].currentDecision = NONE;
				if (!collisionExistWithOtherModels(models[i], models, i)) {
					models[i].teleport(models[i].getCenter() + glm::vec3(2.0f, 0.5f, 0.0f));
					models[i].comingFrom = LEFT;
				} else {
					models[i].comingFrom = NONE;
				}
			}
		}
		else if (horizontalStepCounter[i] <= -monsterStep) {
			models[i].lookLeft();
			if (!collisionExist(models[i])) {
				horizontalStepCounter[i] = 0; verticalStepCounter[i] = 0;
				models[i].currentDecision = NONE;
				if (!collisionExistWithOtherModels(models[i], models, i)) {
					models[i].teleport(models[i].getCenter() + glm::vec3(-2.0f, 0.5f, 0.0f));
					models[i].comingFrom = RIGHT;
				} else {
					models[i].comingFrom = NONE;
				}
			}
		}
	}
	viewMatrix = glm::lookAt(position, position + direction, up);
	for (unsigned i = 1; i < monsterCount + 1; i++) {
		if (models[0].x_center == models[i].x_center && models[0].z_center == models[i].z_center) {
			return true;
		}
	}
	return false;
}

bool Control::collisionExistWithOtherModels(Model &sourceModel, std::vector<Model> &otherModels, int sourceModelIndex) {
	glm::vec3 modelTargetPoint;
	if (sourceModel.lookingTop) { modelTargetPoint = glm::vec3(sourceModel.x_center, 0, sourceModel.z_center + 2); }
	else if (sourceModel.lookingBottom) { modelTargetPoint = glm::vec3(sourceModel.x_center, 0, sourceModel.z_center - 2); }
	else if (sourceModel.lookingRight) { modelTargetPoint = glm::vec3(sourceModel.x_center + 2, 0, sourceModel.z_center); }
	else if (sourceModel.lookingLeft) { modelTargetPoint = glm::vec3(sourceModel.x_center - 2, 0, sourceModel.z_center); }
	for (unsigned i = 0; i < otherModels.size(); i++) {
		if (i != 0 && i != sourceModelIndex && modelTargetPoint.x == otherModels[i].x_center && modelTargetPoint.z == otherModels[i].z_center) {
			return true;
		}
	}
	return false;
}

//---------------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////
// CALCULATING POSSIBLE DIRECTIONS FOR MONSTERS / AI ~ STARTING //
//////////////////////////////////////////////////////////////////

int Control::calculatePossibleDirections(const Model &monster, bool &leftAllowed, bool &rightAllowed, bool &topAllowed, bool &bottomAllowed) {
	if (monster.currentDecision != NONE) {
		if (monster.currentDecision == BOTTOM) { bottomAllowed = true; }
		else if (monster.currentDecision == TOP) { topAllowed = true; }
		else if (monster.currentDecision == RIGHT) { rightAllowed = true; }
		else if (monster.currentDecision == LEFT) { leftAllowed = true; }
		return 1;
	}
	int totalMoves = 0;
	int xCurrIndex = (-1) * monster.z_center / 2 + 9;
	int zCurrIndex = monster.x_center / 2 + 9;
	if (xCurrIndex < floorExistence.size() - 1 && floorExistence[xCurrIndex + 1][zCurrIndex]) {
		totalMoves++;
		bottomAllowed = true;
	}
	if (xCurrIndex > 0 && floorExistence[xCurrIndex - 1][zCurrIndex]) {
		totalMoves++;
		topAllowed = true;
	}
	if (zCurrIndex < floorExistence.size() - 1 && floorExistence[xCurrIndex][zCurrIndex + 1]) {
		totalMoves++;
		rightAllowed = true;
	}
	if (zCurrIndex > 0 && floorExistence[xCurrIndex][zCurrIndex - 1]) {
		totalMoves++;
		leftAllowed = true;
	}
	if (totalMoves >= 2) {
		if (bottomAllowed && monster.comingFrom == BOTTOM) { bottomAllowed = false; totalMoves--; }
		if (topAllowed && monster.comingFrom == TOP) { topAllowed = false; totalMoves--; }
		if (rightAllowed && monster.comingFrom == RIGHT) { rightAllowed = false; totalMoves--; }
		if (leftAllowed && monster.comingFrom == LEFT) { leftAllowed = false; totalMoves--; }
	}
	return totalMoves;
}

//---------------------------------------------------------------------------------------------------

////////////////////////////////////////////////////////
// CALCULATING DIJKSTRA SHORTEST PATH / AI ~ STARTING //
////////////////////////////////////////////////////////

int Control::calculateShortest(Model pacman, Model monster, int index, std::vector<int> &monsterClosestNextX, std::vector<int> &monsterClosestNextZ) {
	// Get the current position of the pacman
	int pacman_x = (-1) * pacman.z_center / 2 + 9;
	int pacman_z = pacman.x_center / 2 + 9;

	// Set the source and destination points on the grid
	int source_x = (-1) * monster.z_center / 2 + 9;
	int source_z = monster.x_center / 2 + 9;

	int dest_x = pacman_x;
	int dest_z = pacman_z;

	struct SearchNode {
		int distance; int x; int z;
		bool operator<(const SearchNode &rhs) const { return distance < rhs.distance; }
		bool operator>(const SearchNode &rhs) const { return distance > rhs.distance; }
		bool operator<=(const SearchNode &rhs) const { return distance <= rhs.distance; }
		bool operator>=(const SearchNode &rhs) const { return distance >= rhs.distance; }
		bool operator==(const SearchNode &rhs) const { return distance == rhs.distance; }
		bool operator!=(const SearchNode &rhs) const { return distance != rhs.distance; }
	};
	const int dx[4] = { -1, 0, 1, 0 };
	const int dz[4] = { 0, 1, 0, -1 };

	// Use Dijkstra's algorithm to find the shortest path from the source to the destination
	std::priority_queue<SearchNode, std::vector<SearchNode>, std::greater<SearchNode>> pq;
	std::vector<std::vector<int>> dist((int)floorCoordinates.size(), std::vector<int>((int)floorCoordinates.size(), std::numeric_limits<int>::max()));
	std::vector<std::vector<int>> prev((int)floorCoordinates.size(), std::vector<int>((int)floorCoordinates.size(), -1));
	// Push the source point onto the priority queue
	pq.push({ 0, source_x, source_z });
	dist[source_x][source_z] = 0;
	while (!pq.empty()) {
		SearchNode curr = pq.top();
		pq.pop();
		int x = curr.x;
		int z = curr.z;
		if (x == dest_x && z == dest_z) {
			// Stop the search when the destination point is reached
			break;
		}
		for (int i = 0; i < 4; i++) {
			// Check the four neighboring cells
			int new_x = x + dx[i];
			int new_z = z + dz[i];
			if (new_x >= 0 && new_x < (int)floorCoordinates.size() && new_z >= 0 && new_z < (int)floorCoordinates.size() && floorExistence[new_x][new_z]) {
				// Update the distance and previous cell if a shorter path to this cell is found
				int newDist = dist[x][z] + 1;
				if (newDist < dist[new_x][new_z]) {
					dist[new_x][new_z] = newDist;
					prev[new_x][new_z] = i;
					// Push the updated cell onto the priority queue
					pq.push({ newDist, new_x, new_z });
				}
			}
		}
	}

	// Follow the shortest path to the pacman
	int x = dest_x;
	int z = dest_z;
	while (prev[x][z] != -1) {
		// Follow the path backwards from the destination to the source
		int prev_x = x - dx[prev[x][z]];
		int prev_z = z - dz[prev[x][z]];
		if (prev[prev_x][prev_z] == -1) { break; }
		x = prev_x;
		z = prev_z;
	}

	monsterClosestNextX[index] = x;
	monsterClosestNextZ[index] = z;
	return dist[dest_x][dest_z];
}
