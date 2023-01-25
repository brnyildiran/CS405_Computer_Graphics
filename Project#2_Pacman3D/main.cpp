#define GLEW_STATIC

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "shader.h"
#include "control.h"
#include "world.h"
#include "model.h"
#include "texture.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

//////////////////////////////////////////////////////////////////////////
// HELPER CLASSES & VARIABLES & METHODS FOR TEXT RENDERING ~ STARTING ////
//////////////////////////////////////////////////////////////////////////
// A struct that stores information about each character in the font.
struct Character {
	unsigned int TextureID;
	glm::ivec2   Size;
	glm::ivec2   Bearing;
	unsigned int Advance;
};

// A map that stores information about each character in the font.
std::map<GLchar, Character> Characters;
GLuint textBuffer;

// Renders text on the screen using the given Shader, text, position, scale, and color.
void renderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color) {
	shader.activate();
	shader.setVec3("textColor", color);
	glActiveTexture(GL_TEXTURE0);
	shader.setMat4("projectionMatrix", glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f));
	std::string::const_iterator c;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];
		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, textBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(0);
		x += (ch.Advance >> 6) * scale;
	}
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);
}
//---------------------------------------------------------------------------------------------------

///////////////////
// MAIN FUNCTION //
///////////////////

int main() {

	/////////////////////////////////////////////////////////////////
	// Initialize GLFW and creating a window for the application ////
	/////////////////////////////////////////////////////////////////

	GLFWwindow *window;
	if (!glfwInit()) {
		std::cout << "Error while initializing GLFW!" << std::endl;
		return -1;
	}

	// Set GLFW hints for the window
	// Number of samples for anti-aliasing, OpenGL version, Forward compatibility, Core profile
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int width = 1920;
	int height = 1080;
	int fps = 60;
	int shadowWidth = 1600;
	int shadowHeight = 1600;

	// Create the window
	window = glfwCreateWindow(width, height, "OpenGLProject", glfwGetPrimaryMonitor(), NULL);
	if (window == NULL) {
		std::cout << "An error occured!" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Make the context of the window current
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		std::cout << "Error while initializing GLEW!" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Enable sticky keys
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Disable the cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	// Poll events for the window
	glfwPollEvents();

	// Set the cursor position in the center of the window
	glfwSetCursorPos(window, width / 2, height / 2);

	// Clear the color buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Set the depth function
	glDepthFunc(GL_LESS);

	//---------------------------------------------------------------------------------------------------

	//////////////////////////////////
	// SHADER PROGRAMS ~ STARTING ////
	//////////////////////////////////
	Shader shader("shadowMapVertexShader.glsl", "shadowMapFragmentShader.glsl");
	Shader simpleShader("standardVertexShader.glsl", "standardFragmentShader.glsl");
	Shader shadingShader("shadingVertexShader.glsl", "shadingFragmentShader.glsl");
	Shader minimapShader("minimapVertexShader.glsl", "minimapFragmentShader.glsl");
	Shader textShader("textVertexShader.glsl", "textFragmentShader.glsl");

	//---------------------------------------------------------------------------------------------------

	///////////////////////////////////
	// INITIALIZING WORLD ~ STARTING //
	///////////////////////////////////
	World world("./coordinates.txt");

	// Vertices
	std::vector<glm::vec3> vertices = world.vertices;

	// Textures
	std::vector<glm::vec2> textures;

	// Colors
	std::vector<glm::vec3> colors = world.colors;

	// Normals
	std::vector<glm::vec3> normals = world.normals;

	// Wall & Light Bulb Information
	// Existence (Bool) and Locations (x,y,z)
	std::vector<std::vector<bool>> wallExistence = world.wallExistence;
	std::vector<std::vector<glm::vec3>> wallLocations = world.wallLocations;
	std::vector<std::vector<bool>> bulbExistence = world.bulbExistence;
	std::vector<std::vector<glm::vec3>> bulbLocations = world.bulbLocations;
	std::vector<std::vector<bool>> floorExistence = world.floorExistence;
	std::vector<std::vector<glm::vec3>> floorLocations = world.floorLocations;
	bulbExistence[9][5] = false;
	bulbExistence[8][9] = false;
	bulbExistence[9][8] = false;
	bulbExistence[9][9] = false;
	bulbExistence[9][10] = false;

	// Pushing a default texture value of (0.0,0.0) in the textures vector for each vertex.
	for (unsigned i = 0; i < vertices.size(); i++) { textures.push_back(glm::vec2(0.0f, 0.0f)); }

	//---------------------------------------------------------------------------------------------------

	///////////////////////////////
	// LOADING MODELS ~ STARTING //
	///////////////////////////////

	// Initialize a vector of "Model" objects
	std::vector<Model> models;

	// PACMAN 
	models.push_back(Model("./models/Pacman", "Pacman.obj"));
	models[0].teleport(glm::vec3(-8.0f, 0.45f, 0.0f));
	models[0].rotateQuarterCycle();
	models[0].setSize(0.625f);

	// RED MONSTER 
	models.push_back(Model("./models/Red_Monster", "Red_Monster.obj"));
	models[1].teleport(glm::vec3(-2.0f, 0.45f, 0.0f));
	models[1].lookRight();
	models[1].setSize(0.625f);
	for (unsigned i = 0; i < models[1].mTextureData.size(); i++) { models[1].mTextureData[i] = glm::vec2(0.0f, 0.0f); }

	// ORANGE MONSTER 
	models.push_back(Model("./models/Orange_Monster", "Orange_Monster.obj"));
	models[2].teleport(glm::vec3(2.0f, 0.45f, 0.0f));
	models[2].rotateQuarterCycle();
	models[2].lookLeft();
	models[2].setSize(0.625f);
	for (unsigned i = 0; i < models[2].mTextureData.size(); i++) { models[2].mTextureData[i] = glm::vec2(0.0f, 0.0f); }

	// CYAN MONSTER 
	models.push_back(Model("./models/Cyan_Monster", "Cyan_Monster.obj"));
	models[3].teleport(glm::vec3(0.0f, 0.45f, 0.0f));
	models[3].rotateQuarterCycle();
	models[3].rotateQuarterCycle();
	models[3].rotateQuarterCycle();
	models[3].lookTop();
	models[3].setSize(3.50f);
	for (unsigned i = 0; i < models[3].mTextureData.size(); i++) { models[3].mTextureData[i] = glm::vec2(0.0f, 0.0f); }

	// PACMAN COOKIE
	Model pacmanCookie("./models/Pacman_Cookie", "Pacman_Cookie.obj");
	pacmanCookie.teleport(glm::vec3(0.0f, 2.0f, 0.0f));
	pacmanCookie.lookRight();
	pacmanCookie.rotateQuarterCycle();
	pacmanCookie.rotateQuarterCycle();
	pacmanCookie.rotateQuarterCycle();
	pacmanCookie.setSize(12.5f);
	for (unsigned i = 0; i < pacmanCookie.mTextureData.size(); i++) { pacmanCookie.mTextureData[i] = glm::vec2(0.0f, 0.0f); }

	// BULBS
	// Generate light bulbs in the given locations, remove bulbs from the screen when collected, will be used for tracking the score
	int bulbsTotal = 0;
	std::vector<std::vector<Model>> bulbs(bulbExistence.size(), std::vector<Model>(bulbExistence.size(), Model("./models/Light_Bulb", "Light_Bulb.obj")));
	for (unsigned i = 0; i < bulbExistence.size(); i++) {
		for (unsigned j = 0; j < bulbExistence[i].size(); j++) {
			if (bulbExistence[i][j]) {
				for (unsigned k = 0; k < bulbs[i][j].mTextureData.size(); k++) { bulbs[i][j].mTextureData[k] = glm::vec2(0.0f, 0.0f); }
				bulbs[i][j].teleport(bulbLocations[i][j]);
				bulbs[i][j].setSize(2.25f);
				bulbsTotal++;
			}
		}
	}

	//---------------------------------------------------------------------------------------------------

	///////////////////////////////
	// LOADING CAMERA ~ STARTING //
	///////////////////////////////
	
	
	std::vector<glm::vec3> wallsCoordinates;
	for (unsigned i = 0; i < wallLocations.size(); i++) {
		for (unsigned j = 0; j < wallLocations[i].size(); j++) {
			if (wallLocations[i][j].x == 0.0f && wallLocations[i][j].y == 0.0f && wallLocations[i][j].z == 0.0f) {
				continue;
			}
			float x = wallLocations[i][j].x;
			float z = wallLocations[i][j].z;
			wallsCoordinates.push_back(glm::vec3(x - 0.5, 0, z - 0.5));
			wallsCoordinates.push_back(glm::vec3(x - 0.5, 0, z + 0.5));
			wallsCoordinates.push_back(glm::vec3(x + 0.5, 0, z - 0.5));
			wallsCoordinates.push_back(glm::vec3(x + 0.5, 0, z + 0.5));
		}
	}
	Control control(window, wallExistence, wallLocations, floorExistence, floorLocations, 3, wallsCoordinates);
	Difficulty currentDifficulty = NORMAL;
	bool failed = control.computeMatrices(models, bulbExistence, bulbLocations, currentDifficulty);
	
	//---------------------------------------------------------------------------------------------------

	///////////////////////////////////////
	// LOADING TEXT RENDERING ~ STARTING //
	///////////////////////////////////////

	// Initialize the FreeType library
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return -1;
	}

	// Define the font
	std::string font_name = "Bungee-Regular.ttf";
	if (font_name.empty()) {
		std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
		return -1;
	}

	// Create a new face object from the specified font
	FT_Face face;
	if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return -1;
	}
	else {
		FT_Set_Pixel_Sizes(face, 0, 48);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		for (unsigned char c = 0; c < 128; c++) {
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			Character character = {
					texture,
					glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					static_cast<unsigned int>(face->glyph->advance.x)
			};
			Characters.insert(std::pair<char, Character>(c, character));
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	
	//---------------------------------------------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PACMAN COOKIE - RENDERING 3D MODEL AS TEXTURE ~ STARTING //
	//////////////////////////////////////////////////////////////

	// Create vectors to store vertex, texture, color, and normal data for the cookie model
	std::vector<glm::vec3> cookieVertices;
	std::vector<glm::vec2> cookieTextures;
	std::vector<glm::vec3> cookieColors;
	std::vector<glm::vec3> cookieNormals;

	// Fill the vectors with the corresponding data from the pacmanCookie object
	pacmanCookie.addVertexData(cookieVertices);
	pacmanCookie.addTextureData(cookieTextures);
	pacmanCookie.addColorData(cookieColors);
	pacmanCookie.addNormalData(cookieNormals);

	// Cookie framebuffer
	GLuint cookieFramebuffer;
	glGenFramebuffers(1, &cookieFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, cookieFramebuffer);

	// Cookie Textures
	GLuint cookieTexture;
	glGenTextures(1, &cookieTexture);
	glBindTexture(GL_TEXTURE_2D, cookieTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 100, 100, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, cookieTexture, 0);

	// Cookie VBO Vertex
	GLuint cookie_VBO_vertex;
	glGenBuffers(1, &cookie_VBO_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, cookie_VBO_vertex);
	glBufferData(GL_ARRAY_BUFFER, cookieVertices.size() * sizeof(glm::vec3), &cookieVertices[0], GL_STATIC_DRAW);

	// Cookie VBO Texture
	GLuint cookie_VBO_texture;
	glGenBuffers(1, &cookie_VBO_texture);
	glBindBuffer(GL_ARRAY_BUFFER, cookie_VBO_texture);
	glBufferData(GL_ARRAY_BUFFER, cookieTextures.size() * sizeof(glm::vec2), &cookieTextures[0], GL_STATIC_DRAW);

	// Cookie VBO Color
	GLuint cookie_VBO_color;
	glGenBuffers(1, &cookie_VBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, cookie_VBO_color);
	glBufferData(GL_ARRAY_BUFFER, cookieColors.size() * sizeof(glm::vec3), &cookieColors[0], GL_STATIC_DRAW);

	// Cookie Normal 
	GLuint cookie_VBO_normal;
	glGenBuffers(1, &cookie_VBO_normal);
	glBindBuffer(GL_ARRAY_BUFFER, cookie_VBO_normal);
	glBufferData(GL_ARRAY_BUFFER, cookieNormals.size() * sizeof(glm::vec3), &cookieNormals[0], GL_STATIC_DRAW);
	
	//---------------------------------------------------------------------------------------------------

	/////////////////////////////////
	// PREPARING RENDER ~ STARTING //
	/////////////////////////////////
	// Adding the data of vertices, textures, colors, normals to models and bulbs

	// MODELS
	for (unsigned i = 0; i < models.size(); i++) {
		models[i].addVertexData(vertices);
		models[i].addTextureData(textures);
		models[i].addColorData(colors);
		models[i].addNormalData(normals);
	}

	// BULBS
	for (unsigned i = 0; i < bulbExistence.size(); i++) {
		for (unsigned j = 0; j < bulbExistence[i].size(); j++) {
			if (bulbExistence[i][j]) {
				bulbs[i][j].addVertexData(vertices);
				bulbs[i][j].addTextureData(textures);
				bulbs[i][j].addColorData(colors);
				bulbs[i][j].addNormalData(normals);
			}
		}
	}
	
	//---------------------------------------------------------------------------------------------------

	////////////////////////////////////////////
	// CREATING BUFFERS FOR RENDER ~ STARTING //
	////////////////////////////////////////////

	// Create a VAO to store vertex buffer binding state
	GLuint vertexArray;
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	// Create a buffer for texture coordinates
	glGenBuffers(1, &textBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, textBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_STATIC_DRAW);
	
	// Create a buffer for vertex positions
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	
	// Create a buffer for texture coordinates
	GLuint textureBuffer;
	glGenBuffers(1, &textureBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(glm::vec2), &textures[0], GL_STATIC_DRAW);
	
	// Create a buffer for vertex colors
	GLuint colorBuffer;
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), &colors[0], GL_STATIC_DRAW);
	
	// Create a buffer for vertex normals
	GLuint normalBuffer;
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	
	//---------------------------------------------------------------------------------------------------

	///////////////////////////////////////////////////
	// CREATING BUFFERS FOR SHADOWMAPPING ~ STARTING //
	///////////////////////////////////////////////////
	// Generate a framebuffer object for storing depth information
	GLuint depthMapFramebuffer;
	glGenFramebuffers(1, &depthMapFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFramebuffer);

	// Generate a texture object to hold the textures of the depth map
	GLuint depthTexture;
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	// Texture parameters for the depth map
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	// Attach the depth map texture to the framebuffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
	glDrawBuffer(GL_NONE);

	// Check if the framebuffer is complete and return false if not
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return false;
	}
	
	//---------------------------------------------------------------------------------------------------

	/////////////////////////////////////////////
	// CREATING BUFFERS FOR MINIMAP ~ STARTING //
	/////////////////////////////////////////////

	// Generate a framebuffer object
	GLuint shadingFramebuffer;
	glGenFramebuffers(1, &shadingFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shadingFramebuffer);

	// Generate a texture object to hold the rendered image
	GLuint renderedTexture;
	glGenTextures(1, &renderedTexture);
	glBindTexture(GL_TEXTURE_2D, renderedTexture);

	// Allocate storage for the rendered texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 300, 300, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Set texture parameters for the rendered texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Attach the rendered texture to the framebuffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

	// Define the vertices for a quad to be used for displaying the rendered texture
	std::vector<glm::vec3> quadVertices = {
		glm::vec3(-1.0f, -1.0f, 0.0f),
		glm::vec3(1.0f, -1.0f, 0.0f),
		glm::vec3(-1.0f, 1.0f, 0.0f),
		glm::vec3(-1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, -1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f)
	};

	// Generate a vertex buffer to hold the quad vertices
	GLuint quadVertexBuffer;
	glGenBuffers(1, &quadVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
	
	//---------------------------------------------------------------------------------------------------

	// LOADING TEXTURE FOR PACMAN
	GLuint texture = loadTexture("./models/Pacman/Pacman.jpg");

	//---------------------------------------------------------------------------------------------------

	//////////////////////////
	// GAME LOOP ~ STARTING //
	//////////////////////////
	int livesRemaining = 3;
	int bulbsCollected = 0;
	double lastTime = glfwGetTime();
	do {
		double currentTime = glfwGetTime();
		if (currentTime - lastTime >= 1.0 / fps) {
			//////////////////////////////////////////////////
			// FILLING BUFFERS FOR PACMAN COOKIE ~ STARTING //
			//////////////////////////////////////////////////
			glViewport(0, 0, 100, 100);
			glBindFramebuffer(GL_FRAMEBUFFER, cookieFramebuffer);
			glm::mat4 modelMatrix = glm::mat4(1.0); modelMatrix[0][0] = -1;
			shadingShader.activate();
			shadingShader.setMat4("projectionMatrix", glm::ortho<float>(-20, 20, -20, 20, -20, 20));
			shadingShader.setMat4("viewMatrix", glm::lookAt(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
			shadingShader.setMat4("modelMatrix", modelMatrix);

			// VBO Vertex
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, cookie_VBO_vertex);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);

			// VBO Texture
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, cookie_VBO_texture);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void *)0);

			// VBO Color
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, cookie_VBO_color);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);

			// VBO Normal
			glEnableVertexAttribArray(3);
			glBindBuffer(GL_ARRAY_BUFFER, cookie_VBO_normal);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);

			// Render the cookie
			glBindBuffer(GL_ARRAY_BUFFER, cookie_VBO_vertex);
			glBufferData(GL_ARRAY_BUFFER, cookieVertices.size() * sizeof(glm::vec3), &cookieVertices[0], GL_STATIC_DRAW);
			glDrawArrays(GL_TRIANGLES, 0, cookieVertices.size());

			// Disable Vertex Attribute Arrays
			// Vertex, Texture, Color, Normal
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
			glDisableVertexAttribArray(3);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			
			//---------------------------------------------------------------------------------------------------

			///////////////////////////////////////////////////////////////////
			// UPDATING COORDINATES / Collecting Bulbs and 3 Lives~ STARTING //
			///////////////////////////////////////////////////////////////////
			lastTime = currentTime;

			// Remove Bulbs from the screen
			for (unsigned i = 0; i < bulbExistence.size(); i++) {
				for (unsigned j = 0; j < bulbExistence[i].size(); j++) {
					if (bulbExistence[i][j]) {
						bulbs[i][j].removeVertexData(vertices);
					}
				}
			}
			for (unsigned i = 0; i < models.size(); i++) {
				models[i].removeVertexData(vertices);
			}

			// Checking for FAIL conditions --> 3 Lives
			failed = control.computeMatrices(models, bulbExistence, bulbLocations, currentDifficulty);
			if (failed) {
				livesRemaining -= 1;
				// When player has no lives left
				if (livesRemaining == 0) {
					std::cout << "You failed the round!" << std::endl;
					break;
				}
				// If player has remaining lives
				// Teleports and resets the position, direction and decision of the models
				models[0].teleport(glm::vec3(-8.0f, 0.45f, 0.0f));
				models[1].teleport(glm::vec3(-2.0f, 0.45f, 0.0f));
				models[1].lookRight();
				models[1].comingFrom = LEFT;
				models[1].currentDecision = NONE;
				models[2].teleport(glm::vec3(2.0f, 0.45f, 0.0f));
				models[2].lookLeft();
				models[2].comingFrom = RIGHT;
				models[2].currentDecision = NONE;
				models[3].teleport(glm::vec3(0.0f, 0.45f, 0.0f));
				models[3].lookTop();
				models[3].comingFrom = BOTTOM;
				models[3].currentDecision = NONE;
				control.setStartPosition();
			}

			// Checks if all bulbs have been collected
			if (bulbsCollected == bulbsTotal) {
				std::cout << "Round is completed!" << std::endl;
				break;
			}

			// Adds the vertex data for the models
			for (unsigned i = 0; i < models.size(); i++) {
				models[i].addVertexData(vertices);
			}

			// Number of Bulbs collected
			int bulbsInThisFrame = 0;
			for (unsigned i = 0; i < bulbExistence.size(); i++) {
				for (unsigned j = 0; j < bulbExistence[i].size(); j++) {
					if (bulbExistence[i][j]) {
						bulbs[i][j].addVertexData(vertices);
						bulbsInThisFrame++;
					}
				}
			}

			// Count of total collected Bulbs --> Game Score
			bulbsCollected = bulbsTotal - bulbsInThisFrame;
			
			//---------------------------------------------------------------------------------------------------

			////////////////////////////////////////////////////////////////////////////
			// CALCULATING DEPTH FOR SHADOWMAPPING / Shadow Map & Lighting ~ STARTING //
			////////////////////////////////////////////////////////////////////////////

			// Direction of the light source
			glm::vec3 lightInvDirection = glm::vec3(-0.0f, 4.0f, 2.0f);

			// Projection & View matrix for the depth buffer
			glm::mat4 depthProjectionMatrix = glm::ortho<float>(-20, 20, -20, 20, -20, 20);
			glm::mat4 depthViewMatrix = glm::lookAt(lightInvDirection, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFramebuffer);

			// Clear the color & depth buffers
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Viewport size for the depth buffer
			glViewport(0, 0, shadowWidth, shadowHeight);

			// Activate the simpleShader program
			simpleShader.activate();
			glm::mat4 depthModelMatrix = glm::mat4(1.0); depthModelMatrix[0][0] = -1;
			glm::mat4 depthSpaceMatrix = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
			simpleShader.setMat4("depthSpaceMatrix", depthSpaceMatrix);

			// Enable Backface Culling
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);

			// Texture
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);

			// Vertex operations
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
			glDrawArrays(GL_TRIANGLES, 0, vertices.size());
			glDisableVertexAttribArray(0);

			// Unbinds the depth buffer framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// Disable Backface Culling
			glDisable(GL_CULL_FACE);

			// Clear the color & depth buffers
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			//---------------------------------------------------------------------------------------------------

			///////////////////////////////////////////////////
			// CALCULATING PROJECTION FOR MINIMAP ~ STARTING //
			///////////////////////////////////////////////////
			glViewport(0, 0, 300, 300);
			glBindFramebuffer(GL_FRAMEBUFFER, shadingFramebuffer);

			// Projection - View - Space matrices
			glm::mat4 projectionMatrix = control.getProjectionMatrix();
			glm::mat4 viewMatrix = control.getViewMatrix();
			glm::mat4 spaceMatrix = projectionMatrix * viewMatrix * modelMatrix;

			// Activate the shadingShader program
			shadingShader.activate();
			shadingShader.setMat4("projectionMatrix", glm::ortho<float>(-20, 20, -20, 20, -20, 20));
			shadingShader.setMat4("viewMatrix", glm::lookAt(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
			shadingShader.setMat4("modelMatrix", modelMatrix);

			// Vertex
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);

			// Texture
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void *)0);

			// Color
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);

			// Normal
			glEnableVertexAttribArray(3);
			glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);

			// Render the vertex data
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
			glDrawArrays(GL_TRIANGLES, 0, vertices.size());

			// Disable Vertex Attribute Arrays
			// Vertex, Texture, Color, Normal
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
			glDisableVertexAttribArray(3);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			
			//---------------------------------------------------------------------------------------------------

			////////////////////////////////////////
			// RENDERING CURRENT FRAME ~ STARTING //
			////////////////////////////////////////
			// Rendering the scene with shadow effect
			glViewport(0, 0, width, height);

			// Activate the Shader
			shader.activate();

			// Bias & depthBiasSpaceMatrix
			glm::mat4 biasMatrix(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0);
			glm::mat4 depthBiasSpaceMatrix = biasMatrix * depthSpaceMatrix;

			// Set the space, view, and model matrices in the shader
			shader.setMat4("spaceMatrix", spaceMatrix);
			shader.setMat4("viewMatrix", viewMatrix);
			shader.setMat4("modelMatrix", modelMatrix);

			// Set the light direction in world space in the shader
			shader.setVec3("lightInvDirectionWorldSpace", lightInvDirection);

			// Set the depth bias space matrix in the shader
			shader.setMat4("depthBiasSpaceMatrix", depthBiasSpaceMatrix);

			// Set the texture and shadow map in the shader
			shader.setInt("samplingTexture", 0);
			shader.setInt("shadowMap", 1);

			// Bind the texture and depth texture to the corresponding texture units
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, depthTexture);

			// Vertex
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);

			// Texture
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void *)0);

			// Color
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);

			// Normal
			glEnableVertexAttribArray(3);
			glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);

			// Render the vertex data
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
			glDrawArrays(GL_TRIANGLES, 0, vertices.size());

			// Disable Vertex Attribute Arrays
			// Vertex, Texture, Color, Normal
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
			glDisableVertexAttribArray(3);
			
			//---------------------------------------------------------------------------------------------------

			//////////////////////////////////
			// RENDERING MINIMAP ~ STARTING //
			//////////////////////////////////
			glViewport(0, height - 300, 300, 300);

			// Activate minimapShader
			minimapShader.activate();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, renderedTexture);
			minimapShader.setInt("renderedTexture", 0);

			// Render the Mini Map
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(glm::vec3), &quadVertices[0], GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
			glDrawArrays(GL_TRIANGLES, 0, quadVertices.size());

			// Disable the vertex array and the BLEND mode
			glDisableVertexAttribArray(0);
			glDisable(GL_BLEND);
			
			//---------------------------------------------------------------------------------------------------

			/////////////////////////////////////////////
			// RENDERING TEXT / Score Board ~ STARTING //
			/////////////////////////////////////////////
			glViewport(0, 0, width, height);
			std::string text = "Score: ";
			if (bulbsCollected < 10) {
				text += "0";
			}	
			if (bulbsCollected < 100) {
				text += "0";
			}
			text += std::to_string(bulbsCollected) + "/" + std::to_string(bulbsTotal);
			renderText(textShader, text, 510, 1005, 1, glm::vec3(1.0f, 1.0f, 1.0f));
			
			if (currentDifficulty == EASY) {
				renderText(textShader, "DIFFICULTY: EASY", 1000, 1005, 1, glm::vec3(1.0f, 1.0f, 1.0f));
			}	else if (currentDifficulty == NORMAL) {
				renderText(textShader, "DIFFICULTY: NORMAL", 1000, 1005, 1, glm::vec3(1.0f, 1.0f, 1.0f));
			} else if (currentDifficulty == HARD) {
				renderText(textShader, "DIFFICULTY: HARD", 1000, 1005, 1, glm::vec3(1.0f, 1.0f, 1.0f));
			}

			//---------------------------------------------------------------------------------------------------

			///////////////////////////////////////
			// RENDERING PACMAN LIVES ~ STARTING //
			///////////////////////////////////////
			glViewport(width - 100, height - 100, 100, 100);

			// Activate minimapShader
			minimapShader.activate();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, cookieTexture);
			minimapShader.setInt("renderedTexture", 0);

			// Render the Mini Map
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(glm::vec3), &quadVertices[0], GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
			glDrawArrays(GL_TRIANGLES, 0, quadVertices.size());
			glDisableVertexAttribArray(0);
			glDisable(GL_BLEND);
			// If you have more than 2 lives
			if (livesRemaining >= 2) {
				glViewport(width - 200, height - 100, 100, 100);
				minimapShader.activate();
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, cookieTexture);
				minimapShader.setInt("renderedTexture", 0);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
				glEnableVertexAttribArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
				glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(glm::vec3), &quadVertices[0], GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
				glDrawArrays(GL_TRIANGLES, 0, quadVertices.size());
				glDisableVertexAttribArray(0);
				glDisable(GL_BLEND);
			}
			// If you have more than 3 lives
			if (livesRemaining >= 3) {
				glViewport(width - 300, height - 100, 100, 100);
				minimapShader.activate();
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, cookieTexture);
				minimapShader.setInt("renderedTexture", 0);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
				glEnableVertexAttribArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
				glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(glm::vec3), &quadVertices[0], GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
				glDrawArrays(GL_TRIANGLES, 0, quadVertices.size());
				glDisableVertexAttribArray(0);
				glDisable(GL_BLEND);
			}
			
			//---------------------------------------------------------------------------------------------------

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	//////////////////////////////////////
	// CLEANING-UP RESOURCES & MEMORY ////
	//////////////////////////////////////
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &textureBuffer);
	glDeleteBuffers(1, &normalBuffer);
	glDeleteBuffers(1, &colorBuffer);
	glDeleteBuffers(1, &depthMapFramebuffer);
	glDeleteProgram(shader.id);
	glDeleteProgram(simpleShader.id);
	glDeleteProgram(shadingShader.id);
	glDeleteProgram(minimapShader.id);
	glDeleteProgram(textShader.id);
	glDeleteVertexArrays(1, &vertexArray);
	glfwTerminate();
	return 0;
}