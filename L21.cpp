// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <functional>
#include <GL/glew.h>
// Include GLFW
#include <GLFW/glfw3.h>
#include "shader.hpp"
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.hpp"
#include <iostream>
#include <vector>
#include <array>
#include "LoadBMP.hpp"
#include "camera.h"
std::vector<float> interleaved;
bool mousePressed = false;
double lastX, lastY;
float radius = 20.0f;
float cameraSpeed = 0.3f;
GLuint VBO;
GLuint VAO;
GLuint waterTextureID, displacementTextureID;
GLuint waterShaderProgramID;
glm::vec3 cameraPos = glm::vec3(12.716, 15.0885, 16.1214);
glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

GLFWwindow *windowInMain;

void checkForError(std::string value)
{

	GLenum error;

	while ((error = glGetError()) != GL_NO_ERROR)
	{
		std::cerr << "OpenGL error " << value << " " << error << std::endl;
	}
}
// Camera camera = Camera(eye, center, up, cameraYaw, cameraPitch, cameraSpeed, rotateDeg);
Camera camera(cameraPos, target, worldUp);
void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
{
	if (mousePressed)
	{
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		float speedOfDrag = 0.1f;
		xoffset *= speedOfDrag;
		yoffset *= speedOfDrag;

		camera.processRotation(xoffset, yoffset);

		lastX = xpos;
		lastY = ypos;
	}
}
void handleInput(GLFWwindow *window, glm::vec3 &lpos, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		camera.processRadiusUpdate(-1 * cameraSpeed * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		camera.processRadiusUpdate(cameraSpeed * deltaTime);
	}
	
}
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
			mousePressed = true;
			glfwGetCursorPos(window, &lastX, &lastY);
		}
		else if (action == GLFW_RELEASE)
		{
			mousePressed = false;
		}
	}
}

void setMyShaderUniforms(glm::mat4 &model, glm::mat4 &VIEW, glm::mat4 &Projection, glm::vec3 &lightpos)
{
	glm::mat4 MVP = Projection * VIEW * model;
	glm::vec3 modelColor = glm::vec3(0.384f, 0.839f, 0.843f);
	float texScale = 7.0f;			 
	glm::vec2 texOffset(0.0f, 0.0f);
	float time = glfwGetTime();
	glUniformMatrix4fv(glGetUniformLocation(waterShaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	checkForError("model");
	glUniformMatrix4fv(glGetUniformLocation(waterShaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(VIEW));
	checkForError("view");

	glUniformMatrix4fv(glGetUniformLocation(waterShaderProgramID, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
	checkForError("MVP");

	glUniform3fv(glGetUniformLocation(waterShaderProgramID, "cameraPos"), 1, glm::value_ptr(camera.getCameraPos()));
	checkForError("cameraPos");

	glUniform3fv(glGetUniformLocation(waterShaderProgramID, "lightPos"), 1, glm::value_ptr(lightpos));
	checkForError("lightPos");

	glUniform2fv(glGetUniformLocation(waterShaderProgramID, "texOffset"), 1, glm::value_ptr(texOffset));
	checkForError("texOffset");

	glUniform1f(glGetUniformLocation(waterShaderProgramID, "texScale"), texScale);
	checkForError("texScale");

	glUniform1f(glGetUniformLocation(waterShaderProgramID, "time_vs"), time);
	checkForError("time_vs");

	glUniformMatrix4fv(glGetUniformLocation(waterShaderProgramID, "MVP_gs"), 1, GL_FALSE, glm::value_ptr(MVP));
	checkForError("MVP_gs");

	glUniformMatrix4fv(glGetUniformLocation(waterShaderProgramID, "V_gs"), 1, GL_FALSE, glm::value_ptr(VIEW));
	checkForError("V_gs");

	glUniformMatrix4fv(glGetUniformLocation(waterShaderProgramID, "P_gs"), 1, GL_FALSE, glm::value_ptr(Projection));
	checkForError("P_gs");

	glUniformMatrix4fv(glGetUniformLocation(waterShaderProgramID, "M_gs"), 1, GL_FALSE, glm::value_ptr(model));
	checkForError("M_gs");

	float outer = 16.0f;
	float inner = 16.0f;
	glUniform1f(glGetUniformLocation(waterShaderProgramID, "outerTess"), outer);
	checkForError("outer");
	glUniform1f(glGetUniformLocation(waterShaderProgramID, "innerTess"), inner);
	checkForError("inner");
	checkForError("before");
	glUniform1i(glGetUniformLocation(waterShaderProgramID, "waterTexture"), 0);
	checkForError("waterTexture");
	glUniform1i(glGetUniformLocation(waterShaderProgramID, "displacementTexture"), 1);
	checkForError("dispText");
}

void loadMyTexture()
{

	unsigned char *data;

	unsigned int width, height;
	loadBMP("Assets/water.bmp", &data, &width, &height);
	// fprintf(stderr, "w: %d, h: %d\n", width, height);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &waterTextureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	free(data);
	loadBMP("Assets/displacement-map1.bmp", &data, &width, &height);
	// fprintf(stderr, "w: %d, h: %d\n", width, height);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &displacementTextureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, displacementTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	free(data);
}

std::vector<float> verts, normals;
std::vector<int> indices;
void planeMeshQuads(float min, float max, float stepsize)
{

	// The following coordinate system works as if (min, 0, min) is the origin
	// And then builds up the mesh from that origin down (in z)
	// and then to the right (in x).
	// So, one "row" of the mesh's vertices have a fixed x and increasing z

	// manually create a first column of vertices
	float x = min;
	float y = 0;
	for (float z = min; z <= max; z += stepsize)
	{
		verts.push_back(x);
		verts.push_back(y);
		verts.push_back(z);
		normals.push_back(0);
		normals.push_back(1);
		normals.push_back(0);

		interleaved.push_back(x);
		interleaved.push_back(y);
		interleaved.push_back(z);

		interleaved.push_back(0);
		interleaved.push_back(1);
		interleaved.push_back(0);

		// interleaved.push_back(u);
		// interleaved.push_back(v);
	}

	for (float x = min + stepsize; x <= max; x += stepsize)
	{
		for (float z = min; z <= max; z += stepsize)
		{
			verts.push_back(x);
			verts.push_back(y);
			verts.push_back(z);
			normals.push_back(0);
			normals.push_back(1);
			normals.push_back(0);

			interleaved.push_back(x);
			interleaved.push_back(y);
			interleaved.push_back(z);

			interleaved.push_back(0);
			interleaved.push_back(1);
			interleaved.push_back(0);

			// interleaved.push_back(u);
			// interleaved.push_back(v);
		}
	}

	int nCols = (max - min) / stepsize + 1;
	int i = 0, j = 0;
	for (float x = min; x < max; x += stepsize)
	{
		j = 0;
		for (float z = min; z < max; z += stepsize)
		{
			indices.push_back(i * nCols + j);
			indices.push_back(i * nCols + j + 1);
			indices.push_back((i + 1) * nCols + j + 1);
			indices.push_back((i + 1) * nCols + j);
			++j;
		}
		++i;
	}

}

//////////////////////////////////////////////////////////////////////////////
// Main
//////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{

	///////////////////////////////////////////////////////
	float screenW = 1400;
	float screenH = 900;

	if (argc > 1)
	{
		screenW = atoi(argv[1]);
	}
	if (argc > 2)
	{
		screenH = atoi(argv[2]);
	}

	///////////////////////////////////////////////////////

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}
	glfwWindowHint(GLFW_SAMPLES, 4);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	windowInMain = glfwCreateWindow(screenW, screenH, "Assignment 6", NULL, NULL);
	if (windowInMain == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(windowInMain);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(windowInMain, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.2f, 0.2f, 0.3f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDisable(GL_CULL_FACE);

	// Projection = glm::mat4(1.0f);
	//glMatrixMode(GL_PROJECTION);
	//glPushMatrix();
	// Projection = glm::mat4(1.0f);
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), screenW / screenH, 0.001f, 1000.0f);
	//glLoadMatrixf(glm::value_ptr(Projection));

	camera.updateCoords();

	glm::mat4 VIEW = camera.getViewMat();

	//glMatrixMode(GL_MODELVIEW);
	//glPushMatrix();
	//glLoadMatrixf(glm::value_ptr(VIEW));
	// glm::vec3 lightpos(15.0f, 15.0f, 15.0f);
	glm::vec3 lightpos(12.9171, 125.936, -134.213);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	float stepsize = 1.0f;

	float xmin = -10;
	float xmax = 10;

	// glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	planeMeshQuads(xmin, xmax, stepsize);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, interleaved.size() * sizeof(float), interleaved.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);

	glEnableVertexAttribArray(1);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);

	loadMyTexture();
	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	GLenum error;
	// Axes ax(target, glm::vec3(4.0f, 4.0f, 4.0f));

	while ((error = glGetError()) != GL_NO_ERROR)
	{

		std::cerr << "OpenGL error " << error << std::endl;
	}
	int cnt = 0;
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	glfwSetMouseButtonCallback(windowInMain, mouseButtonCallback);
	// Set the cursor position callback function
	glfwSetCursorPosCallback(windowInMain, cursorPositionCallback);
	waterShaderProgramID = LoadShadersGeo("CG3.vertexshader", "CG3.geometryshader", "CG3.fragmentshader");

	glm::mat4 model = glm::mat4(1.0f);
	int max = 5, min = -5;
	// if (glIsProgram(waterShaderProgramID) == GL_TRUE)
	// {
	// 	printf("Shader ID %d is valid\n", waterShaderProgramID);
	// }
	// else
	// {
	// 	printf("Shader ID %d is invalid\n", waterShaderProgramID);
	// }
	glUseProgram(waterShaderProgramID);
	glPatchParameteri(GL_PATCH_VERTICES, 4); 
	glUseProgram(0);

	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// ax.draw();
		checkForError("clear" + std::to_string(cnt));

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		handleInput(windowInMain, lightpos, deltaTime);
		checkForError("window");

		// drawCubeBorders({xmax, xmax, xmax}, {xmin, xmin, xmin});

		glUseProgram(waterShaderProgramID);
		checkForError("shader" + std::to_string(cnt));

		VIEW = camera.getViewMat();
		setMyShaderUniforms(model, VIEW, Projection, lightpos);

		glBindVertexArray(VAO);
		checkForError("VAO" + std::to_string(cnt));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, waterTextureID);
		checkForError("TEXTURE ACTIVE IN " + std::to_string(cnt));

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, displacementTextureID);
		checkForError("TEXTURE ACTIVE IN " + std::to_string(cnt));

		// glDrawArrays(GL_QUADS, 0, totalVertices);

		glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, 0);

		checkForError("TDRAW " + std::to_string(cnt));
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		checkForError("UNBIND " + std::to_string(cnt));

		glBindVertexArray(0);

		glUseProgram(0);
		checkForError("UNBIND  SHADE" + std::to_string(cnt));

		//glMatrixMode(GL_MODELVIEW);

		//glLoadMatrixf(glm::value_ptr(VIEW));
		// Swap buffers
		glfwSwapBuffers(windowInMain);
		glfwPollEvents();
		checkForError("LOOP End" + std::to_string(cnt));

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(windowInMain, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		   glfwWindowShouldClose(windowInMain) == 0);

	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteProgram(waterShaderProgramID);

	glfwTerminate();
	return 0;
}
