﻿//GAME2012_FinalProject_DasShila.cpp by Shila Das(C) 2020 All Rights Reserved.
//
// Final Project submission.
//
// Description:
// Click run to see the results.
//
//*****************************************************************************

using namespace std;

#include <cstdlib>
#include <ctime>
#include "vgl.h"
#include "LoadShaders.h"
#include "Light.h"
#include "Shape.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define FPS 60
#define MOVESPEED 0.1f
#define TURNSPEED 0.05f
#define X_AXIS glm::vec3(1,0,0)
#define Y_AXIS glm::vec3(0,1,0)
#define Z_AXIS glm::vec3(0,0,1)
#define XY_AXIS glm::vec3(1,1,0)
#define YZ_AXIS glm::vec3(0,1,1)
#define XZ_AXIS glm::vec3(1,0,1)


enum keyMasks {
	KEY_FORWARD =  0b00000001,		// 0x01 or 1 or 01
	KEY_BACKWARD = 0b00000010,		// 0x02 or 2 or 02
	KEY_LEFT = 0b00000100,		
	KEY_RIGHT = 0b00001000,
	KEY_UP = 0b00010000,
	KEY_DOWN = 0b00100000,
	KEY_MOUSECLICKED = 0b01000000
	// Any other keys you want to add.
};

// IDs.
GLuint vao, ibo, points_vbo, colors_vbo, uv_vbo, normals_vbo, modelID, viewID, projID;
GLuint program;

// Matrices.
glm::mat4 View, Projection;

// Our bitflags. 1 byte for up to 8 keys.
unsigned char keys = 0; // Initialized to 0 or 0b00000000.

// Camera and transform variables.
float scale = 1.0f, angle = 0.0f;
glm::vec3 position, frontVec, worldUp, upVec, rightVec; // Set by function
GLfloat pitch, yaw;
int lastX, lastY;

// Texture variables.
GLuint firstTx, blankTx, Tx, wallTx, gateTx, gateTopTx, mazeTx;
GLint width, height, bitDepth;

// Light variables.
AmbientLight aLight(glm::vec3(1.0f, 1.0f, 1.0f),	// Ambient colour.
	0.5f);							// Ambient strength.

DirectionalLight dLight(glm::vec3(1.0f, 0.0f, 0.0f), // Direction.
	glm::vec3(1.0f, 1.0f, 0.5f),  // Diffuse colour.
	0.1f);						  // Diffuse strength.

PointLight pLights[2] = { { glm::vec3(7.5f, 1.0f, -5.0f), //position
							10.0f, //range
							glm::vec3(1.0f, 0.0f, 0.0f), //diffuse color
							5.0f }, //dffuse strength
						  { glm::vec3(2.5f, 0.5f, -5.0f), 15.0f, glm::vec3(0.0f, 0.0f, 1.0f), 5.0f } };

SpotLight sLight(glm::vec3(5.0f, 25.75f, -5.0f),	// Position.
	glm::vec3(1.0f, 1.0f, 1.0f),	// Diffuse colour.
	1.0f,							// Diffuse strength.
	glm::vec3(0.0f, -1.0f, 0.0f),  // Direction.
	15.0f);

void timer(int);

void resetView()
{
	position = glm::vec3(5.0f, 3.0f, 10.0f);
	frontVec = glm::vec3(0.0f, 0.0f, -1.0f);
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	pitch = 0.0f;
	yaw = -90.0f;
	// View will now get set only in transformObject
}
glm::vec3 itemPosition;
Cube g_cube(1);
Cube wall_cube(1);
Prism wall_prism(24);
Cone wall_cone(24);
Cube g_cubeS(1);
Prism g_prism(12);
Plane g_plane;
//Crystal g_crystal;
Grid g_grid(10, 3); // New UV scale parameter. Works with texture now.
bool showItem = true;
void setupLights()
{
	// Setting ambient Light.
	glUniform3f(glGetUniformLocation(program, "aLight.ambientColour"), aLight.ambientColour.x, aLight.ambientColour.y, aLight.ambientColour.z);
	glUniform1f(glGetUniformLocation(program, "aLight.ambientStrength"), aLight.ambientStrength);

	// Setting directional light.
	glUniform3f(glGetUniformLocation(program, "dLight.base.diffuseColour"), dLight.diffuseColour.x, dLight.diffuseColour.y, dLight.diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "dLight.base.diffuseStrength"), dLight.diffuseStrength);

	glUniform3f(glGetUniformLocation(program, "dLight.direction"), dLight.direction.x, dLight.direction.y, dLight.direction.z);

	// Setting point lights.
	glUniform3f(glGetUniformLocation(program, "pLights[0].base.diffuseColour"), pLights[0].diffuseColour.x, pLights[0].diffuseColour.y, pLights[0].diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "pLights[0].base.diffuseStrength"), pLights[0].diffuseStrength);
	glUniform3f(glGetUniformLocation(program, "pLights[0].position"), pLights[0].position.x, pLights[0].position.y, pLights[0].position.z);
	glUniform1f(glGetUniformLocation(program, "pLights[0].constant"), pLights[0].constant);
	glUniform1f(glGetUniformLocation(program, "pLights[0].linear"), pLights[0].linear);
	glUniform1f(glGetUniformLocation(program, "pLights[0].exponent"), pLights[0].exponent);

	glUniform3f(glGetUniformLocation(program, "pLights[1].base.diffuseColour"), pLights[1].diffuseColour.x, pLights[1].diffuseColour.y, pLights[1].diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "pLights[1].base.diffuseStrength"), pLights[1].diffuseStrength);
	glUniform3f(glGetUniformLocation(program, "pLights[1].position"), pLights[1].position.x, pLights[1].position.y, pLights[1].position.z);
	glUniform1f(glGetUniformLocation(program, "pLights[1].constant"), pLights[1].constant);
	glUniform1f(glGetUniformLocation(program, "pLights[1].linear"), pLights[1].linear);
	glUniform1f(glGetUniformLocation(program, "pLights[1].exponent"), pLights[1].exponent);

	// Setting spot light.
	glUniform3f(glGetUniformLocation(program, "sLight.base.diffuseColour"), sLight.diffuseColour.x, sLight.diffuseColour.y, sLight.diffuseColour.z);
	glUniform1f(glGetUniformLocation(program, "sLight.base.diffuseStrength"), sLight.diffuseStrength);

	glUniform3f(glGetUniformLocation(program, "sLight.position"), sLight.position.x, sLight.position.y, sLight.position.z);

	glUniform3f(glGetUniformLocation(program, "sLight.direction"), sLight.direction.x, sLight.direction.y, sLight.direction.z);
	glUniform1f(glGetUniformLocation(program, "sLight.edge"), sLight.edgeRad);
}
void loadTexture(GLuint &texture, char *filename, bool pngFile=false)
{
	unsigned char* image = stbi_load(filename, &width, &height, &bitDepth, 0);
	if (!image) cout << "Unable to load file!" << endl;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	if(pngFile)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image);
}
void init(void)
{
	srand((unsigned)time(NULL));
	//Specifying the name of vertex and fragment shaders.
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "triangles.vert" },
		{ GL_FRAGMENT_SHADER, "triangles.frag" },
		{ GL_NONE, NULL }
	};

	//Loading and compiling shaders
	program = LoadShaders(shaders);
	glUseProgram(program);	//My Pipeline is set up

	modelID = glGetUniformLocation(program, "model");
	projID = glGetUniformLocation(program, "projection");
	viewID = glGetUniformLocation(program, "view");

	// Projection matrix : 45∞ Field of View, aspect ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(45.0f), 1.0f / 1.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	// Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	resetView();

	// Image loading.
	stbi_set_flip_vertically_on_load(true);

	loadTexture(firstTx, "grid.png",true);
	loadTexture(wallTx, "wall.jpg");	
	loadTexture(gateTx, "gate.jpg");	
	loadTexture(gateTopTx, "gatetop.jpg");	
	loadTexture(mazeTx, "grass.png",true);	
	loadTexture(blankTx, "blank.jpg");
	loadTexture(Tx, "cube.jpg");
	glUniform1i(glGetUniformLocation(program, "texture0"), 0);

	setupLights();

	vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

		ibo = 0;
		glGenBuffers(1, &ibo);
	
		points_vbo = 0;
		glGenBuffers(1, &points_vbo);

		colors_vbo = 0;
		glGenBuffers(1, &colors_vbo);

		uv_vbo = 0;
		glGenBuffers(1, &uv_vbo);

		normals_vbo = 0;
		glGenBuffers(1, &normals_vbo);

	glBindVertexArray(0); // Can optionally unbind the vertex array to avoid modification.

	// Change shape data.
	g_prism.SetMat(0.1, 16);
	g_grid.SetMat(0.0, 16);

	// Enable depth test and blend.
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	// Enable face culling.
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	itemPosition = glm::vec3(7.5f, 1.0f, -8.5f);
	timer(0); 
}

//---------------------------------------------------------------------
//
// calculateView
//
void calculateView()
{
	frontVec.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	frontVec.y = sin(glm::radians(pitch));
	frontVec.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	frontVec = glm::normalize(frontVec);
	rightVec = glm::normalize(glm::cross(frontVec, worldUp));
	upVec = glm::normalize(glm::cross(rightVec, frontVec));

	View = glm::lookAt(
		position, // Camera position
		position + frontVec, // Look target
		upVec); // Up vector
	glUniform3f(glGetUniformLocation(program, "eyePosition"), position.x, position.y, position.z);
}
void itemDistance()
{
	if (showItem)
	{
		float d = (itemPosition.x - position.x) * (itemPosition.x - position.x)
			+ (itemPosition.y - position.y) * (itemPosition.y - position.y)
			+ (itemPosition.z - position.z) * (itemPosition.z - position.z);
		if (d < 1.0f)
		{
			cout << "got item" << endl;
			itemPosition = glm::vec3(4.1f, 0.5f, -4.7f);
			position = glm::vec3(4.2f, 0.4f, -5.0f);
			showItem = false;
		}
	}
}
//---------------------------------------------------------------------
//
// transformModel
//
void transformObject(glm::vec3 scale, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 translation) {
	glm::mat4 Model;
	Model = glm::mat4(1.0f);
	Model = glm::translate(Model, translation);
	Model = glm::rotate(Model, glm::radians(rotationAngle), rotationAxis);
	Model = glm::scale(Model, scale);
	
	calculateView();
	itemDistance();
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(projID, 1, GL_FALSE, &Projection[0][0]);
}
void createShape(GLuint texture, Shape s, glm::vec3 scale, glm::vec3 translation)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	s.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(scale, X_AXIS, 0.0f, translation);
	glDrawElements(GL_TRIANGLES, s.NumIndices(), GL_UNSIGNED_SHORT, 0);
}
void drawGate()
{
	//middle
	//gate
	createShape(gateTx, g_plane, glm::vec3(2.0f, 2.5f, 1.0f), glm::vec3(4.0f, 0.8f, -0.5f));
}
void drawGateTop()
{
	//gate top
	createShape(gateTopTx, wall_cube, glm::vec3(2.0f, 1.0f, 0.3f), glm::vec3(4.0f, 3.0f, -0.5f));	
	//gate top
	createShape(gateTopTx, wall_cube, glm::vec3(4.0f, 0.5f, 0.3f), glm::vec3(3.0f, 4.0f, -0.5f));	
}
void drawStair()
{
	//stair
	createShape(wallTx, wall_cube, glm::vec3(2.0f, 0.2f, 0.2f), glm::vec3(4.0f, 0.6f, -0.5f));
	createShape(wallTx, wall_cube, glm::vec3(2.0f, 0.2f, 0.2f), glm::vec3(4.0f, 0.4f, -0.3f));
	createShape(wallTx, wall_cube, glm::vec3(2.0f, 0.2f, 0.2f), glm::vec3(4.0f, 0.2f, -0.1f));
	createShape(wallTx, wall_cube, glm::vec3(2.0f, 0.2f, 0.2f), glm::vec3(4.0f, 0.0f, 0.1f));	
}
void drawWallBundary()
{
	//left
	//wall_cube
	createShape(wallTx, wall_cube, glm::vec3(0.5f, 2.5f, 8.0f), glm::vec3(0.0f, 0.0f, -9.0f));	
	//wall top cube
	createShape(wallTx, wall_cube, glm::vec3(0.1f, 0.5f, 1.0f), glm::vec3(0.0f, 2.5f, -7.0f));
	createShape(wallTx, wall_cube, glm::vec3(0.1f, 0.5f, 1.0f), glm::vec3(0.0f, 2.5f, -5.0f));
	createShape(wallTx, wall_cube, glm::vec3(0.1f, 0.5f, 1.0f), glm::vec3(0.0f, 2.5f, -3.0f));
	createShape(wallTx, wall_cube, glm::vec3(0.1f, 0.5f, 1.0f), glm::vec3(0.0f, 2.5f, -9.0f));	
	//back
	//wall_cube
	createShape(wallTx, wall_cube, glm::vec3(9.5f, 2.5f, 0.5f), glm::vec3(0.3f, 0.0f, -9.8f));	
	//top cube
	createShape(wallTx, wall_cube, glm::vec3(1.0f, 0.5f, 0.1f), glm::vec3(1.5f, 2.5f, -9.8f));
	createShape(wallTx, wall_cube, glm::vec3(1.0f, 0.5f, 0.1f), glm::vec3(3.5f, 2.5f, -9.8f));
	createShape(wallTx, wall_cube, glm::vec3(1.0f, 0.5f, 0.1f), glm::vec3(5.5f, 2.5f, -9.8f));
	createShape(wallTx, wall_cube, glm::vec3(1.0f, 0.5f, 0.1f), glm::vec3(7.5f, 2.5f, -9.8f));	

	//right
	//wall Cube
	createShape(wallTx, wall_cube, glm::vec3(0.5f, 2.5f, 8.0f), glm::vec3(10.0f, 0.0f, -9.0f));	
	//wall top
	createShape(wallTx, wall_cube, glm::vec3(0.1f, 0.5f, 1.0f), glm::vec3(10.4f, 2.5f, -7.0f));
	createShape(wallTx, wall_cube, glm::vec3(0.1f, 0.5f, 1.0f), glm::vec3(10.4f, 2.5f, -5.0f));
	createShape(wallTx, wall_cube, glm::vec3(0.1f, 0.5f, 1.0f), glm::vec3(10.4f, 2.5f, -3.0f));
	createShape(wallTx, wall_cube, glm::vec3(0.1f, 0.5f, 1.0f), glm::vec3(10.4f, 2.5f, -9.0f));
	
	//front 
	//wall_cube
	createShape(wallTx, wall_cube, glm::vec3(3.0f, 3.0f, 0.3f), glm::vec3(0.5f, 0.0f, -0.5f));
	//wall_cube
	createShape(wallTx, wall_cube, glm::vec3(3.0f, 3.0f, 0.3f), glm::vec3(6.5f, 0.0f, -0.5f));		
}
void drawWallCorner()
{
	//left
	//wall cylinder
	createShape(wallTx, wall_prism, glm::vec3(1.0f, 3.0f, 1.0f), glm::vec3(-0.5f, 0.0f, -1.0f));	
	//wall top cone
	createShape(wallTx, wall_cone, glm::vec3(1.5f, 1.0f, 1.5f), glm::vec3(-0.75f, 3.0f, -1.25f));	
	//wall cylinder
	createShape(wallTx, wall_prism, glm::vec3(1.0f, 3.0f, 1.0f), glm::vec3(-0.5f, 0.0f, -10.0f));	
	//wall top cone
	createShape(wallTx, wall_cone, glm::vec3(1.5f, 1.0f, 1.5f), glm::vec3(-0.75f, 3.0f, -10.25f));	
	//right
	//wall cylinder
	createShape(wallTx, wall_prism, glm::vec3(1.0f, 3.0f, 1.0f), glm::vec3(9.5f, 0.0f, -1.0f));	
	//wall top cone
	createShape(wallTx, wall_cone, glm::vec3(1.5f, 1.0f, 1.5f), glm::vec3(9.25f, 3.0f, -1.25f));	
	//wall cylinder
	createShape(wallTx, wall_prism, glm::vec3(1.0f, 3.0f, 1.0f), glm::vec3(9.5f, 0.0f, -10.0f));	
	//wall top cone
	createShape(wallTx, wall_cone, glm::vec3(1.5f, 1.0f, 1.5f), glm::vec3(9.25f, 3.0f, -10.25f));	
}
void drawGateSide()
{
	//gate
	//left
	createShape(wallTx, wall_prism, glm::vec3(1.0f, 4.0f, 1.0f), glm::vec3(3.2f, 0.0f, -0.7f));	
	//gate right
	createShape(wallTx, wall_prism, glm::vec3(1.0f, 4.0f, 1.0f), glm::vec3(5.7f, 0.0f, -0.7f));	
}
void drawMaze()
{
	//Now Maze
	//row 0
	createShape(mazeTx, wall_cube, glm::vec3(7.0f, 2.0f, 0.5f), glm::vec3(2.0f, 0.0f, -1.5f));	
	//row 1
	createShape(mazeTx, wall_cube, glm::vec3(0.5f, 2.0f, 1.0f), glm::vec3(2.0f, 0.0f, -2.5f));
	createShape(mazeTx, wall_cube, glm::vec3(0.5f, 2.0f, 1.0f), glm::vec3(4.0f, 0.0f, -2.5f));
	createShape(mazeTx, wall_cube, glm::vec3(0.5f, 2.0f, 1.0f), glm::vec3(6.0f, 0.0f, -2.5f));
	createShape(mazeTx, wall_cube, glm::vec3(1.0f, 2.0f, 0.5f), glm::vec3(6.0f, 0.0f, -2.5f));	
	//row 2
	createShape(mazeTx, wall_cube, glm::vec3(0.5f, 2.0f, 1.0f), glm::vec3(3.0f, 0.0f, -3.5f));
	createShape(mazeTx, wall_cube, glm::vec3(3.0f, 2.0f, 0.5f), glm::vec3(2.0f, 0.0f, -3.5f));
	createShape(mazeTx, wall_cube, glm::vec3(2.0f, 2.0f, 0.5f), glm::vec3(6.0f, 0.0f, -3.5f));
	createShape(mazeTx, wall_cube, glm::vec3(0.5f, 2.0f, 2.0f), glm::vec3(8.0f, 0.0f, -3.5f));	

	//row 3
	createShape(mazeTx, wall_cube, glm::vec3(3.0f, 2.0f, 0.5f), glm::vec3(1.0f, 0.0f, -4.5f));
	createShape(mazeTx, wall_cube, glm::vec3(0.5f, 2.0f, 1.0f), glm::vec3(6.0f, 0.0f, -4.5f));
	createShape(mazeTx, wall_cube, glm::vec3(1.0f, 2.0f, 0.5f), glm::vec3(6.0f, 0.0f, -4.5f));

	//row 4
	createShape(mazeTx, wall_cube, glm::vec3(2.0f, 2.0f, 0.5f), glm::vec3(2.0f, 0.0f, -5.5f));
	createShape(mazeTx, wall_cube, glm::vec3(0.5f, 2.0f, 3.0f), glm::vec3(5.0f, 0.0f, -5.5f));
	createShape(mazeTx, wall_cube, glm::vec3(3.0f, 2.0f, 0.5f), glm::vec3(5.0f, 0.0f, -5.5f));
	
	//row 5
	createShape(mazeTx, wall_cube, glm::vec3(5.0f, 2.0f, 0.5f), glm::vec3(3.0f, 0.0f, -6.5f));	

	//row 6
	createShape(mazeTx, wall_cube, glm::vec3(0.5f, 2.0f, 2.0f), glm::vec3(2.0f, 0.0f, -7.5f));
	createShape(mazeTx, wall_cube, glm::vec3(5.0f, 2.0f, 0.5f), glm::vec3(2.0f, 0.0f, -7.5f));
	
	//row 7
	createShape(mazeTx, wall_cube, glm::vec3(0.5f, 2.0f, 7.5f), glm::vec3(1.0f, 0.0f, -8.5f));
	createShape(mazeTx, wall_cube, glm::vec3(6.0f, 2.0f, 0.5f), glm::vec3(1.0f, 0.0f, -8.5f));
	createShape(mazeTx, wall_cube, glm::vec3(0.5f, 2.0f, 1.5f), glm::vec3(7.0f, 0.0f, -8.5f));
	createShape(mazeTx, wall_cube, glm::vec3(0.5f, 2.0f, 4.0f), glm::vec3(8.0f, 0.0f, -8.5f));
	createShape(mazeTx, wall_cube, glm::vec3(1.0f, 2.0f, 0.5f), glm::vec3(8.0f, 0.0f, -8.5f));
	createShape(mazeTx, wall_cube, glm::vec3(0.5f, 2.0f, 7.5f), glm::vec3(9.0f, 0.0f, -8.5f));	
}
//---------------------------------------------------------------------
//
// display
//
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vao);
	glUniform3f(glGetUniformLocation(program, "sLight.position"), sLight.position.x, sLight.position.y, sLight.position.z);
	glUniform3f(glGetUniformLocation(program, "pLights[0].position"), pLights[0].position.x, pLights[0].position.y, pLights[0].position.z);
	glUniform3f(glGetUniformLocation(program, "pLights[1].position"), pLights[1].position.x, pLights[1].position.y, pLights[1].position.z);
	// Draw all shapes.

	/*glBindTexture(GL_TEXTURE_2D, alexTx);
	g_plane.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(10.0f, 10.0f, 1.0f), X_AXIS, -90.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	glDrawElements(GL_TRIANGLES, g_plane.NumIndices(), GL_UNSIGNED_SHORT, 0);*/

	
	glBindTexture(GL_TEXTURE_2D, firstTx);
	g_grid.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
	transformObject(glm::vec3(1.0f, 1.0f, 1.0f), X_AXIS, -90.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	glDrawElements(GL_TRIANGLES, g_grid.NumIndices(), GL_UNSIGNED_SHORT, 0);

	drawMaze();
	drawWallCorner();
	drawWallBundary();
	drawGateSide();
	drawGateTop();
	drawGate();
	drawStair();
	//center prism
	createShape(wallTx, g_prism, glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(4.2f, 0.0f, -5.0f));	

	
	//if (showItem)
	//{		
		glBindTexture(GL_TEXTURE_2D, Tx);
		g_cubeS.BufferShape(&ibo, &points_vbo, &colors_vbo, &uv_vbo, &normals_vbo, program);
		transformObject(glm::vec3(0.3f, 0.3f, 0.3f), X_AXIS, angle++, itemPosition);
		glDrawElements(GL_TRIANGLES, g_cubeS.NumIndices(), GL_UNSIGNED_SHORT, 0);
	//}

	glBindVertexArray(0); // Done writing.
	glutSwapBuffers(); // Now for a potentially smoother render.
}

void parseKeys()
{
	if (keys & KEY_FORWARD)
		position += frontVec * MOVESPEED;
	else if (keys & KEY_BACKWARD)
		position -= frontVec * MOVESPEED;
	if (keys & KEY_LEFT)
		position -= rightVec * MOVESPEED;
	else if (keys & KEY_RIGHT)
		position += rightVec * MOVESPEED;
	if (keys & KEY_UP)
		position.y += MOVESPEED;
	else if (keys & KEY_DOWN)
		position.y -= MOVESPEED;
}

void timer(int) { // essentially our update()
	parseKeys();
	glutPostRedisplay();
	glutTimerFunc(1000/FPS, timer, 0); // 60 FPS or 16.67ms.
}

//---------------------------------------------------------------------
//
// keyDown
//
void keyDown(unsigned char key, int x, int y) // x and y is mouse location upon key press.
{
	switch (key)
	{
	case 'w':
		if (!(keys & KEY_FORWARD))
			keys |= KEY_FORWARD; break;
	case 's':
		if (!(keys & KEY_BACKWARD))
			keys |= KEY_BACKWARD; break;
	case 'a':
		if (!(keys & KEY_LEFT))
			keys |= KEY_LEFT; break;
	case 'd':
		if (!(keys & KEY_RIGHT))
			keys |= KEY_RIGHT; break;
	case 'r':
		if (!(keys & KEY_UP))
			keys |= KEY_UP; break;
	case 'f':
		if (!(keys & KEY_DOWN))
			keys |= KEY_DOWN; break;
	case 'i':
		sLight.position.z -= 0.1; break;
	case 'j':
		sLight.position.x -= 0.1; break;
	case 'k':
		sLight.position.z += 0.1; break;
	case 'l':
		sLight.position.x += 0.1; break;
	case 'p':
		sLight.position.y += 0.1; break;
	case ';':
		sLight.position.y -= 0.1; break;
	}
}

void keyDownSpec(int key, int x, int y) // x and y is mouse location upon key press.
{
	switch (key)
	{
	case GLUT_KEY_UP:
		pLights[0].position.y += 0.1;
		pLights[1].position.y += 0.1;
		break;
	case GLUT_KEY_DOWN:
		pLights[0].position.y -= 0.1;
		pLights[1].position.y -= 0.1;
		break;
	case GLUT_KEY_LEFT:
		pLights[0].position.x -= 0.1;
		pLights[1].position.x -= 0.1;
		break;
	case GLUT_KEY_RIGHT:
		pLights[0].position.x += 0.1;
		pLights[1].position.x += 0.1;
		break;
	case GLUT_KEY_PAGE_UP:
		pLights[0].position.z += 0.1;
		pLights[1].position.z += 0.1;
		break;
	case GLUT_KEY_PAGE_DOWN:
		pLights[0].position.z -= 0.1;
		pLights[1].position.z -= 0.1;
	}
}

void keyUp(unsigned char key, int x, int y) // x and y is mouse location upon key press.
{
	switch (key)
	{
	case 'w':
		keys &= ~KEY_FORWARD; break;
	case 's':
		keys &= ~KEY_BACKWARD; break;
	case 'a':
		keys &= ~KEY_LEFT; break;
	case 'd':
		keys &= ~KEY_RIGHT; break;
	case 'r':
		keys &= ~KEY_UP; break;
	case 'f':
		keys &= ~KEY_DOWN; break;
	case ' ':
		resetView();
	}
}

void keyUpSpec(int key, int x, int y) // x and y is mouse location upon key press.
{	
}

void mouseMove(int x, int y)
{
	if (keys & KEY_MOUSECLICKED)
	{
		pitch += (GLfloat)((y - lastY) * TURNSPEED);
		yaw -= (GLfloat)((x - lastX) * TURNSPEED);
		lastY = y;
		lastX = x;
	}
}

void mouseClick(int btn, int state, int x, int y)
{
	if (state == 0)
	{
		lastX = x;
		lastY = y;
		keys |= KEY_MOUSECLICKED; // Flip flag to true
		glutSetCursor(GLUT_CURSOR_NONE);
		//cout << "Mouse clicked." << endl;
	}
	else
	{
		keys &= ~KEY_MOUSECLICKED; // Reset flag to false
		glutSetCursor(GLUT_CURSOR_INHERIT);
		//cout << "Mouse released." << endl;
	}
}

void clean()
{
	cout << "Cleaning up!" << endl;
	glDeleteTextures(1, &firstTx);	
	glDeleteTextures(1, &blankTx);
	glDeleteTextures(1, &Tx);
	glDeleteTextures(1, &gateTopTx);
	glDeleteTextures(1, &gateTx);
	glDeleteTextures(1, &wallTx);
	glDeleteTextures(1, &mazeTx);
}

//---------------------------------------------------------------------
//
// main
//
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
	glutSetOption(GLUT_MULTISAMPLE, 8);
	glutInitWindowSize(900, 900);
	glutCreateWindow("GAME2012_FinalProject_DasShila");

	glewInit();	//Initializes the glew and prepares the drawing pipeline.
	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyDown);
	glutSpecialFunc(keyDownSpec);
	glutKeyboardUpFunc(keyUp); // New function for third example.
	glutSpecialUpFunc(keyUpSpec);

	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMove); // Requires click to register.
	
	atexit(clean); // This GLUT function calls specified function before terminating program. Useful!

	glutMainLoop();

	return 0;
}
