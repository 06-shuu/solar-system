//-----------------------------------------------------------------------------
// solarsystem.cpp 
//  Solar system with sun, moon, and planets.
//-----------------------------------------------------------------------------
#include <iostream>
#include <sstream>
#define GLEW_STATIC
#include "GL/glew.h"	// Important - this header must come before glfw3 header
#include "GLFW/glfw3.h"

#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/gtc/type_ptr.hpp"


//my custom headers
#include "ShaderProgram.h"  
#include "texture.h"
#include "Camera.h"
#include "Mesh.h"
#include "VAO.h"

// Global Variables
const char* APP_TITLE = "Solar System";
 int gWindowWidth = 1800;
 int gWindowHeight = 1200;
GLFWwindow* gWindow = NULL;
bool gWireframe = false;

const std::string skyBoxTex = "res/texture/skyBox";  //location of sky box textures
const std::string suntextureFileName = "res/models/sun.jpg";
const std::string introtextureFileName = "res/texture/introScreen.png";
bool isMoving = true; // when clicking on P (the flag becomes false) the planets will stop moving (p is for pause)
bool introScreenOn = true; // when clicking on I the intro screen will disappear and the solar system scene will start (i is for introduction)


//setting camera
FPSCamera fpsCamera(glm::vec3(0.0f, 0.0f, 17.0f)); //to see the whole thing 

//control the sensitivity of the mouse
const double ZOOM_SENSETIVITY = -3.0f;
const double MOVE_SPEED = 5.0; //unit per second
const double MOUSE_SENSETIVITY = 0.1f;

// Function prototypes
void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY);
void update(double elapsedTime);
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height);
void showFPS(GLFWwindow* window); //
bool initOpenGL();
void zoomInPlanet(glm::vec3 planetPos[], int planetIndex, float zoomFactor); //added 
//void zoomInPlanet(glm::vec3 planetPos[]);

//-----------------------------------------------------------------------------
// Main Application Entry Point
//-----------------------------------------------------------------------------
int main()
{
	if (!initOpenGL())
	{
		// An error occured
		std::cerr << "GLFW initialization failed" << std::endl;
		return -1;
	}
	
	
	//to be printed in the black screen
	std::cout << R"(
		Welcome to the Solar System!
		Here are important notes before you start:
		1. Pressing on 'U' will give an immediate top view.
		2. Pressing on 'J' will zoom in the view.
		3. Pressing on 'P' will stop the orbital motion of the planets
		Press 1 to 9 (without releasing your finger) to zoom into the planets
		)";


	//skybox (gl_cube_map) vertices 
	float skyBoxVertices[] = {
		//   Coordinates
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f
	};

	unsigned int skyBoxIndices[] = {
		1, 2, 6,
		6, 5, 1,
		0, 4, 7,
		7, 3, 0,
		4, 5, 6,
		6, 7, 4,
		0, 3, 2,
		2, 1, 0,
		0, 1, 5,
		5, 4, 0,
		3, 7, 6,
		6, 2, 3
	};

	float introScreenVertices[] = {
		// position            //texture
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
	};

	unsigned int introScreenIndices[] = {
		0, 1, 2,
		2, 3, 0
	};

	//For the skyBox
	unsigned int skyBoxVBO, skyBoxIBO, introVBO, introIBO; 


	// ===================== For the introScreen
	glGenBuffers(1, &introVBO);
	glBindBuffer(GL_ARRAY_BUFFER, introVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(introScreenVertices), introScreenVertices, GL_STATIC_DRAW);

	VAO introVAO;
	introVAO.bind();
	introVAO.Attribpointer(0, 3, GL_FLOAT, 5 * sizeof(GLfloat), (GLvoid*)0);
	introVAO.Attribpointer(1, 2, GL_FLOAT, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//glGenVertexArrays(1, &introVAO);
	//glBindVertexArray(introVAO);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	//glEnableVertexAttribArray(0);
	////for texture 
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(1);


	// Set up index buffer
	glGenBuffers(1, &introIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, introIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(introScreenIndices), introScreenIndices, GL_STATIC_DRAW); 

	// ===================== For the Skybox

	glGenBuffers(1, &skyBoxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyBoxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyBoxVertices), &skyBoxVertices, GL_STATIC_DRAW);

	/*glGenVertexArrays(1, &skyBoxVAO);
	glBindVertexArray(skyBoxVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);*/

	VAO skyBoxVAO;
	skyBoxVAO.bind();
	skyBoxVAO.Attribpointer(0, 3, GL_FLOAT, 3 * sizeof(GLfloat), (GLvoid*)0);

	
	glGenBuffers(1, &skyBoxIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyBoxIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyBoxIndices), &skyBoxIndices, GL_STATIC_DRAW);

	
	//glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind vbo
	//glBindVertexArray(0); //unbind vao
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); //unbind ibo

	// ============= call shaders
	ShaderProgram shaderProgram, lightShader, skyBoxShader, introShader;
	shaderProgram.loadShaders("res/shaders/lighting_phong.vert", "res/shaders/lighting_phong.frag");
	lightShader.loadShaders("res/shaders/sun.vert", "res/shaders/sun.frag");
	skyBoxShader.loadShaders("res/shaders/skyBox.vert", "res/shaders/skyBox.frag");
	introShader.loadShaders("res/shaders/intro.vert", "res/shaders/intro.frag");

	//load meshes 
	const int numModels = 9;
	Mesh mesh[numModels];
	Texture texture[numModels];
	Mesh lightMesh;

	// ============= LOADING THE OBJECTS
	mesh[0].loadOBJ("res/models/mercury.obj");
	mesh[1].loadOBJ("res/models/venus.obj");
	mesh[2].loadOBJ("res/models/earth.obj");
	mesh[3].loadOBJ("res/models/moon.obj");
	mesh[4].loadOBJ("res/models/mars.obj");
	mesh[5].loadOBJ("res/models/jupiter.obj");
	mesh[6].loadOBJ("res/models/saturn.obj");
	mesh[7].loadOBJ("res/models/uranus.obj");
	mesh[8].loadOBJ("res/models/neptune.obj");
	//load mesh for light
	lightMesh.loadOBJ("res/models/sun.obj"); 

	////**********************
	//// TEXTURE
	//// *********************
	Texture sbtexture, sunTexture, circleTexture, introTexture;
	sbtexture.loadCubemap(skyBoxTex, ".jpg", true); //loading the skybox here
	sunTexture.loadTexture(suntextureFileName, true);
	introTexture.loadTexture(introtextureFileName, true);

	texture[0].loadTexture("res/models/mercury.jpg", true);
	texture[1].loadTexture("res/models/venus.jpg", true);
	texture[2].loadTexture("res/models/moon.jpg", true);
	texture[3].loadTexture("res/models/earth.jpg", true);
	texture[4].loadTexture("res/models/mars.jpg", true);
	texture[5].loadTexture("res/models/jupiter.jpg", true);
	texture[6].loadTexture("res/models/saturn2.jpg", true);
	texture[7].loadTexture("res/models/uranus.jpg", true);
	texture[8].loadTexture("res/models/neptune.jpg", true);

	//planets positions in the space
	glm::vec3 planetPos[] = {
		glm::vec3(4.0f, 1.0f, 0.0f), //Mercury
		glm::vec3(8.0f, 1.0f, 0.0f), //Venus
		glm::vec3(12.0f, 1.0f, 0.0f), //Earth
		glm::vec3(14.0f, 1.0f, 0.0f), //Moon
		glm::vec3(16.0f, 1.0f, 0.0f), //Mars
		glm::vec3(20.0f, 1.0f, 0.0f),//Jupiter
		glm::vec3(24.0f, 1.0f, 0.0f), //Saturn
		glm::vec3(28.0f, 1.0f, 0.0f), //Uranus
		glm::vec3(32.0f, 1.0f, 0.0f), //Neptun
	};


	//Model scaling
	glm::vec3 planetScale[] = {
		glm::vec3(0.7f, 0.7f, 0.7f), //Mercury
		glm::vec3(1.5f, 1.5f, 1.5f), //Venus
		glm::vec3(0.3f, 0.3f, 0.3f), //Moon
		glm::vec3(1.7f, 1.7f, 1.7f), //Earth
		glm::vec3(1.3f, 1.3f, 1.3f), //Mars
		glm::vec3(2.2f, 2.2f, 2.2f), //Jupiter
		glm::vec3(2.1f, 2.1f, 2.1f), //Saturn
		glm::vec3(1.8f, 1.8f, 1.8f), //Uranus
		glm::vec3(1.7f, 1.7f, 1.7f) //Neptun
	};

	//planets speeds
	float planetSpeeds[] = {
		0.8f,//Mercury
		0.7f,//Venus
		0.6f,//Moon
		0.6f,//Earth
		0.5f,// Mars
		0.4f,//Jupiter
		0.3f,//Saturn
		0.2f,//Uranus
		0.1f,//Neptun
	};

	//the sun (sunlight pos)
	glm::vec3 sunlightPos(0.0f, 1.0f, 0.0f);
	glm::vec3 sunlightColor(1.0f, 1.0f, 1.0f); //white

	double lastTime = glfwGetTime();
	double totalTimeElapsed = 0.0;
	float factor = 0.0f;

	////**********************
	//// RENDERING LOOP
	//// *********************
	while (!glfwWindowShouldClose(gWindow))
	{
		showFPS(gWindow);
		double cuurentTime = glfwGetTime();
		double deltaTime = cuurentTime - lastTime;
		totalTimeElapsed += deltaTime;


		// Poll for and process events
		glfwPollEvents();
		update(deltaTime);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//===================INTRO SCREEN========================

		if (introScreenOn) //will display an intro screen until the user clicks on i
		{
			introShader.use();

			introTexture.bind(0);
			glUniform1i(glGetUniformLocation(introShader.getProgram(), "introSampler"), 0);

			//glBindVertexArray(introVAO);
			introVAO.bind();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			introVAO.unbind();
			//glBindVertexArray(0);
			introTexture.unbind(0);

			// Swap front and back buffers
			glfwSwapBuffers(gWindow);

			continue; //added continue instead of adding another else statement to display the solar system after the user clicks on i
		}//end of if state.

		//===========================================

		glm::mat4 model(1.0), view(1.0), projection(1.0);
		view = fpsCamera.getViewMatrix();//*
		
		//create the projection matrix
		projection = glm::perspective(glm::radians(fpsCamera.getFOV()), (float)gWindowWidth / gWindowHeight, 0.1f, 100.0f);


		//===================SKYBOX========================

		glDepthFunc(GL_LEQUAL);

		skyBoxShader.use();
		//view = fpsCamera.getViewMatrix();//*
		skyBoxShader.setUniform("view", glm::mat4(glm::mat3(fpsCamera.getViewMatrix()))); //camera pos inside the skybox
		skyBoxShader.setUniform("projection", projection);

		//glBindVertexArray(skyBoxVAO);
		skyBoxVAO.bind();
		sbtexture.bind(0);
		// Get the location of the "skybox" uniform in the shader
		//GLint skyboxLocation = glGetUniformLocation(skyBoxShader.getProgramID(), "skybox");
		skyBoxShader.setUniform(skyBoxShader.getProgramID(), "skybox");
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		skyBoxVAO.unbind();
		//glBindVertexArray(0);
		sbtexture.unbind(0);

		glDepthFunc(GL_LESS);

		//=============================================

		//===========CALL ZOOM IN PLANET FUNCTION========

		//zoomInPlanet(planetPos);
		//we can choose any zooming factor 
		//zoomInPlanet(planetPos, 0, 5.0f); // Zoom in on Mercury with a factor of 5.0
		//zoomInPlanet(planetPos, 1, 10.0f); // Venus
		//zoomInPlanet(planetPos, 2, 10.0f); // Moon
		//zoomInPlanet(planetPos, 3, 10.0f); // Earth
		//zoomInPlanet(planetPos, 4, 10.0f); // Mars
		//zoomInPlanet(planetPos, 5, 10.0f); // Jupiter
		//zoomInPlanet(planetPos, 6, 10.0f); // Saturn
		//zoomInPlanet(planetPos, 7, 10.0f); // Uranus
		//zoomInPlanet(planetPos, 8, 10.0f); // Neptun

		for (int i = 0; i < numModels; i++)
		{
				if(i==0)
					zoomInPlanet(planetPos, i, 5.0f);
				zoomInPlanet(planetPos, i, 10.0f);
				//projection = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 5.0f);
		}
		
		//=================================================

		//================RENDERING THE SUN===============
		//render the sunlight 
		model = glm::translate(glm::mat4(1.0), sunlightPos) * glm::scale(glm::mat4(1.0), glm::vec3(3.0f, 3.0f, 3.0f));
		lightShader.use();
		lightShader.setUniform("projection", projection);
		lightShader.setUniform("view", view);
		lightShader.setUniform("lightColor", sunlightColor);

		// Bind the light texture to texture unit 0
		sunTexture.bind(0);
		glUniform1i(glGetUniformLocation(lightShader.getProgramID(), "lightTexture"), 0);

		lightShader.setUniform("model", model);
		lightShader.setUniform("lightColor", sunlightColor);

		lightMesh.draw();
		sunTexture.unbind(0);
		//=================================================
		 
		//==============RENDERING THE PLANETS==============
		shaderProgram.use();
		view = fpsCamera.getViewMatrix();//*
		//create the projection matrix
		projection = glm::perspective(glm::radians(fpsCamera.getFOV()), (float)gWindowWidth / gWindowHeight, 0.1f, 100.0f);


		//update the camera position
		glm::vec3 viewPos;
		viewPos.x = fpsCamera.getPosition().x;
		viewPos.y = fpsCamera.getPosition().y;
		viewPos.z = fpsCamera.getPosition().z;

		//pass the uniforms to the shaders
		shaderProgram.setUniform("model", model);
		shaderProgram.setUniform("view", view);
		shaderProgram.setUniform("projection", projection);
		//++added
		shaderProgram.setUniform("viewPos", viewPos);
		shaderProgram.setUniform("lightPos", sunlightPos);
		shaderProgram.setUniform("lightColor", sunlightColor);
	
		//note: when commenting this if statement you can see the planets align
		if (isMoving)//if true then move, else stop
		{

			factor += 0.005f;
			for (int i = 0; i < numModels; i++)
			{
				// here to calculate the distance between the sun and each planet
				float distance = glm::length(planetPos[i] - sunlightPos);
				float radius = distance;

				float speeds = planetSpeeds[i];

				// here to calculate the new position of the planet
				float xPos = radius * cos(speeds * factor);
				float zPos = radius * sin(speeds * factor);
				planetPos[i] = glm::vec3(xPos, 1.0f, zPos);

			}
		}

		for (int i = 0; i < numModels; i++) {
			model = glm::translate(glm::mat4(1.0), planetPos[i]) *
				glm::rotate(glm::mat4(1.0), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f))*
				glm::scale(glm::mat4(1.0), planetScale[i]);

			if(i==3)
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			if(i==6)
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			/*if(i==7)
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));*/

			shaderProgram.setUniform("model", model);

			texture[i].bind(0);
			mesh[i].draw();
			texture[i].unbind(0);
		};

		//=================================================


		// Swap front and back buffers
		glfwSwapBuffers(gWindow);

		lastTime = cuurentTime;
	} //end of rendering loop

	//clean up
	//glDeleteVertexArrays(1, &skyBoxVAO);
	glDeleteVertexArrays(1, &skyBoxVBO);
	glDeleteBuffers(1, &skyBoxIBO);
	//glDeleteVertexArrays(1, &introVAO);
	glDeleteVertexArrays(1, &introVBO);
	glDeleteBuffers(1, &introIBO);
	glfwTerminate();

	return 0;
} //end of main

//-----------------------------------------------------------------------------
// Initialize GLFW and OpenGL
//-----------------------------------------------------------------------------
bool initOpenGL()
{
	// Intialize GLFW 
	// GLFW is configured.  Must be called before calling any GLFW functions
	if (!glfwInit())
	{
		// An error occured
		std::cerr << "GLFW initialization failed" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);	// forward compatible with newer versions of OpenGL as they become available but not backward compatible (it will not run on devices that do not support OpenGL 3.3

	// Create an OpenGL 3.3 core, forward compatible context window
	gWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, NULL, NULL);
	if (gWindow == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	// Make the window's context the current one
	glfwMakeContextCurrent(gWindow);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return false;
	}

	// Set the required callback functions
	glfwSetKeyCallback(gWindow, glfw_onKey);
	glfwSetScrollCallback(gWindow, glfw_onMouseScroll);
	glfwSetFramebufferSizeCallback(gWindow, glfw_onFramebufferSize);

	glClearColor(0.23f, 0.38f, 0.47f, 1.0f);

	// Define the viewport dimensions
	glViewport(0, 0, gWindowWidth, gWindowHeight);

	//enable depth testing
	glEnable(GL_DEPTH_TEST);

	return true;
}

//-----------------------------------------------------------------------------
// Is called whenever a key is pressed/released via GLFW
//-----------------------------------------------------------------------------

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	//	glfwSetWindowShouldClose(window, GL_TRUE);

	//if (key == GLFW_KEY_F && action == GLFW_PRESS)
	//{
	//	gWireframe = !gWireframe;
	//	if (gWireframe)
	//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//	else
	//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//}

	//added
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_F:
			gWireframe = !gWireframe;
			if (gWireframe)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case GLFW_KEY_U:
			fpsCamera.move(glm::vec3(0.0f, 30.0f, 0.0f));
			fpsCamera.rotate(0.0f, -90.0f);
			break;

		case GLFW_KEY_J:
			fpsCamera.move(glm::vec3(0.0f, -30.0f, 0.0f));
			break;
		case GLFW_KEY_P:
			isMoving = !isMoving;
			break;
		case GLFW_KEY_I:
			introScreenOn = !introScreenOn;
			break;
		}
	}


}

//-----------------------------------------------------------------------------
// Is called when the window is resized
//-----------------------------------------------------------------------------
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height)
{
	gWindowWidth = width;
	gWindowHeight = height;
	glViewport(0, 0, width, height);
}

//-----------------------------------------------------------------------------
// Mouse Scroll function
//-----------------------------------------------------------------------------
void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY) {

	double fov = fpsCamera.getFOV() + deltaY * ZOOM_SENSETIVITY;
	fov = glm::clamp(fov, 1.0, 120.0);
	fpsCamera.setFOV((float)fov);
}
//-----------------------------------------------------------------------------
// UPDATE
//-----------------------------------------------------------------------------
void update(double elapsedTime) {
	//camera orientation
	double mouseX, mouseY;

	//get current mouse cursor position delta
	glfwGetCursorPos(gWindow, &mouseX, &mouseY);

	//rotate the camera using the delta in mouse diffrences
	fpsCamera.rotate(float(gWindowWidth / 2.0f - mouseX) * MOUSE_SENSETIVITY,
		float(gWindowHeight / 2.0f - mouseY) * MOUSE_SENSETIVITY);

	glfwSetCursorPos(gWindow, gWindowWidth / 2.0f, gWindowHeight / 2.0f);

	//camera movement forward backward
	if (glfwGetKey(gWindow, GLFW_KEY_W) == GLFW_PRESS)
		fpsCamera.move((float)MOVE_SPEED * (float)elapsedTime * fpsCamera.getLook());
	else if (glfwGetKey(gWindow, GLFW_KEY_S) == GLFW_PRESS)
		fpsCamera.move((float)MOVE_SPEED * (float)elapsedTime * -fpsCamera.getLook());

	//camera movement left and right
	if (glfwGetKey(gWindow, GLFW_KEY_A) == GLFW_PRESS)
		fpsCamera.move((float)MOVE_SPEED * (float)elapsedTime * fpsCamera.getRight());
	else if (glfwGetKey(gWindow, GLFW_KEY_D) == GLFW_PRESS)
		fpsCamera.move((float)MOVE_SPEED * (float)elapsedTime * -fpsCamera.getRight());


	//camera movement up and down
	if (glfwGetKey(gWindow, GLFW_KEY_Z) == GLFW_PRESS)
		fpsCamera.move((float)MOVE_SPEED * (float)elapsedTime * fpsCamera.getUp());
	else if (glfwGetKey(gWindow, GLFW_KEY_X) == GLFW_PRESS)
		fpsCamera.move((float)MOVE_SPEED * (float)elapsedTime * -fpsCamera.getUp());

}

//-----------------------------------------------------------------------------
// Code computes the average frames per second, and also the average time it takes
// to render one frame.  These stats are appended to the window caption bar.
//-----------------------------------------------------------------------------
void showFPS(GLFWwindow* window)
{
	static double previousSeconds = 0.0;
	static int frameCount = 0;
	double elapsedSeconds;
	double currentSeconds = glfwGetTime(); // returns number of seconds since GLFW started, as double float

	elapsedSeconds = currentSeconds - previousSeconds;

	// Limit text updates to 4 times per second
	if (elapsedSeconds > 0.25)
	{
		previousSeconds = currentSeconds;
		double fps = (double)frameCount / elapsedSeconds;
		double msPerFrame = 1000.0 / fps;

		// The C++ way of setting the window title
		std::ostringstream outs;
		outs.precision(3);	// decimal places
		outs << std::fixed
			<< APP_TITLE << "    "
			<< "FPS: " << fps << "    "
			<< "Frame Time: " << msPerFrame << " (ms)";
		glfwSetWindowTitle(window, outs.str().c_str());

		// Reset for next average.
		frameCount = 0;
	}

	frameCount++;
}

//added, press 1 to 9 to zoom in the planets 
void zoomInPlanet(glm::vec3 planetPos[], int planetIndex, float zoomFactor) {
	if (glfwGetKey(gWindow, GLFW_KEY_1 + planetIndex) == GLFW_PRESS && planetIndex >= 0 && planetIndex < 9) {
		fpsCamera.setPosition(planetPos[planetIndex] + glm::vec3(0.0f, 0.0f, zoomFactor));
	}
}
//added
//void zoomInPlanet(glm::vec3 planetPos[]) {
//	if (glfwGetKey(gWindow, GLFW_KEY_1 + 0) == GLFW_PRESS) { //Mercury 
//		fpsCamera.setPosition(planetPos[0] + glm::vec3(0.0f, 0.0f, 5.0f));
//	}
//	else if (glfwGetKey(gWindow, GLFW_KEY_2 + 1) == GLFW_PRESS) { //Venus
//		fpsCamera.setPosition(planetPos[1] + glm::vec3(0.0f, 0.0f, 10.0f));
//	}
//	else if (glfwGetKey(gWindow, GLFW_KEY_3 + 2) == GLFW_PRESS) { //Earth
//		fpsCamera.setPosition(planetPos[2] + glm::vec3(0.0f, 0.0f, 10.0f)); 
//	}
//	else if (glfwGetKey(gWindow, GLFW_KEY_4 + 3) == GLFW_PRESS) {//Moon
//		fpsCamera.setPosition(planetPos[3] + glm::vec3(0.0f, 0.0f, 10.0f));
//	}
//	else if (glfwGetKey(gWindow, GLFW_KEY_5 + 4) == GLFW_PRESS) {//Mars
//		fpsCamera.setPosition(planetPos[4] + glm::vec3(0.0f, 0.0f, 10.0f));
//	}
//	else if (glfwGetKey(gWindow, GLFW_KEY_6 + 5) == GLFW_PRESS) {//Jupiter
//		fpsCamera.setPosition(planetPos[5] + glm::vec3(0.0f, 0.0f, 10.0f));
//	}
//	else if (glfwGetKey(gWindow, GLFW_KEY_7 + 6) == GLFW_PRESS) {//Saturn
//		fpsCamera.setPosition(planetPos[6] + glm::vec3(0.0f, 0.0f, 10.0f));
//	}
//	else if (glfwGetKey(gWindow, GLFW_KEY_8 + 7) == GLFW_PRESS) {//Uranus
//		fpsCamera.setPosition(planetPos[7] + glm::vec3(0.0f, 0.0f, 10.0f));
//	}
//	else if (glfwGetKey(gWindow, GLFW_KEY_9 + 8) == GLFW_PRESS) {//Neptun
//		fpsCamera.setPosition(planetPos[8] + glm::vec3(0.0f, 0.0f, 10.0f));
//	}
//}
