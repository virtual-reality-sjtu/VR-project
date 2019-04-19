/*
SJTU - CS230/CS238: Virtual Reality

Homework 1: Hello Teapot

Tasks:

1. compile and get to run
2. wireframe teapot
3. write mouse and keyboard interaction

	Egemen Ertugrul
	egertu@sjtu.edu.cn

*/

//-----------------------------------------------------------------------------
// includes

#include <stdio.h>
#include <cstdlib>
#include <sdlWrapper.h>
#include <shader.h>
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <model.h>

#undef main // Needed because SDL defines its own main
#ifdef WIN32
	#include <windows.h>
#endif

//-----------------------------------------------------------------------------
// some global variables

// Relative path to source files
std::string codeDir = "../";

// Relative path to models directory
std::string modelsDir = codeDir + "models/";

// flag to check if render wireframe or filled
bool	bWireframe = true;

// near clipping plane
float	zNear = 1.0;

// far clipping plane
float	zFar = 100000.0;

// Function Declarations
bool handleEvents(SDL_Event & evt, sdlWrapper & sdlContext);

// ****************************************************************************
// ************            Insert your code here                   ************
// ****************************************************************************

// You can use these global variables to start thinking about how to implement mouse movements
// You do not have to use these if you don't want

// parameters for the navigation
glm::vec3	viewerPosition	(0.0, 0.0, 50.0);
glm::vec3	viewerCenter	(0.0, 0.0, 0.0);
glm::vec3	viewerUp		(0.0, 1.0, 0.0);

// rotation values for the navigation
float	navigationRotation[3] = { 0.0, 0.0, 0.0 };

// position of the mouse when pressed
int		mousePressedX = 0, mousePressedY = 0;
float	lastXOffset = 0.0, lastYOffset = 0.0, lastZOffset = 0.0;
// mouse button states
int		leftMouseButtonActive = 0, middleMouseButtonActive = 0, rightMouseButtonActive = 0;

// ****************************************************************************
// ****************************************************************************
// ****************************************************************************


//---------------------------------------------------------------
// main function
//---------------------------------------------------------------

void main(int argc, char **argv) {
	
	sdlWrapper sdlContext(1280, 960, "Hello World", 0, false);
	SDL_Event evt;

	float aspectRatio = (float)sdlContext.getWidth() / (float)sdlContext.getHeight();

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	printf("\n\nCS230/CS238: Virtual Reality - HW1\n");
	printf("keys:\n\tw\t- toggle wireframe mode\n\tf\t- toggle fullscreen\n\tesc\t- exit\n\n");
	printf("mouse:\n\tleft button\t- rotation\n\tmiddle button\t- panning\n\tright button\t- zoom in and out\n");

	// Shader class instantiation
	Shader shader("../shaders/vertexShader.glsl", "../shaders/fragShader.glsl");

	// ****************************************************************************
	// ************************* Load Your Models Here ****************************
	// ****************************************************************************
	std::cout << "Loading Models ...";
	Model model(modelsDir + "teapot.obj");
	Model axes(modelsDir + "axes.obj");

	std::cout << " Done!" << std::endl;
	// ****************************************************************************
	// ****************************************************************************
	// ****************************************************************************


	// Main rendering loop
	bool running = true;
	while (running) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		running = handleEvents(evt, sdlContext);

		glViewport(0, 0, sdlContext.getWidth(), sdlContext.getHeight());

		// Always call this before you attach something to uniform, or initiate any draw calls
		shader.Use();
		
		// ****************************************************************************
		// ************************** Insert your Code here ***************************
		// ****************************************************************************

		// Setup Projection, Model, and View Matricies
		glm::mat4 projMat = glm::perspective(glm::radians(50.0f), aspectRatio, zNear, zFar);
		glm::vec3 teapotPos = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::mat4 teapotSca = glm::scale(glm::translate(glm::mat4(1.0f), teapotPos), glm::vec3(10.0f));
		glm::mat4 modelMat = glm::rotate(teapotSca, navigationRotation[1], glm::vec3(1.0f, 0.0f, 0.0f));
		modelMat = glm::rotate(modelMat, navigationRotation[0], glm::vec3(0.0f, 1.0f, 0.0f));
		modelMat = glm::translate(modelMat, glm::vec3(lastXOffset / 100, -lastYOffset / 100, -lastZOffset / 100));
		glm::mat4 viewMat = glm::lookAt(viewerPosition, viewerCenter, viewerUp);

		// Attached Projection, Model, and View matricies to the shader
		// In the shader the Proj * View * Model * vertex_coord operation is carried out
		//shader.attachToUniform("projectionMatrix", projMat);
		//shader.attachToUniform("viewMatrix", viewMat);
		//shader.attachToUniform("modelMatrix", modelMat);
		glm::mat4 mv = viewMat * modelMat;
		
		shader.attachToUniform("MVP", projMat * viewMat * modelMat);
		shader.attachToUniform("ModelViewMatrix", viewMat * modelMat);
		//shader.attachToUniform("ProjectionMatrix", projMat);
		
		
		
		//shader.attachToUniform("NormalMatrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
		shader.attachToUniform("NormalMatrix",
			glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
		
		glm::vec4  worldLight = glm::vec4(5.0f, 5.0f, 2.0f, 1.0f);
		shader.attachToUniform("Material.Kd", glm::vec3(0.9f, 0.5f, 0.3f));
		shader.attachToUniform("Light.Ld", glm::vec3(1.0f, 1.0f, 1.0f));
		
		shader.attachToUniform("Light.Position", viewMat * worldLight);
		
		shader.attachToUniform("Material.Ka", glm::vec3(0.9f, 0.5f, 0.3f));
		shader.attachToUniform("Light.La", glm::vec3(0.4f, 0.4f, 0.4f));
		shader.attachToUniform("Material.Ks", glm::vec3(0.8f, 0.8f, 0.8f));
		shader.attachToUniform("Light.Ls", glm::vec3(1.0f, 1.0f, 1.0f));
		shader.attachToUniform("Material.Shininess", 100.0f);
		
		
		shader.Use();

		//axes.Draw(shader);
		model.Draw(shader);

		// ****************************************************************************
		// ****************************************************************************
		// ****************************************************************************
		
		sdlContext.swapbuffer();
	}

		
}

//**************************************************************
// SDL event handler function
//**************************************************************

bool handleEvents(SDL_Event & evt, sdlWrapper & sdlContext) {

	// Poll all events that have occurred
	while (SDL_PollEvent(&evt)) {
		// If Quit ( X in window is pressed)
		if (evt.type == SDL_QUIT) {
			return false;
		}

		// Handle Keyboard events
		if (evt.type == SDL_KEYDOWN) {
			// Quit if escape key is pressed
			if (evt.key.keysym.sym == SDLK_ESCAPE) {
				return false;
			}

			// Toggle Fullscreen
			if (evt.key.keysym.sym == SDLK_f) {
				sdlContext.toggleFullScreen();
			}

			// Toggle Wireframe
			if (evt.key.keysym.sym == SDLK_w) {
				if (bWireframe) {
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					bWireframe = false;
				}
				else {
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					bWireframe = true;
				}
			}
		}

		// ****************************************************************************
		// ************************** Insert your Code here ***************************
		// ****************************************************************************

		// Handle Mouse Click Events
		if (evt.type == SDL_MOUSEBUTTONDOWN) {
			switch (evt.button.button) {
			case SDL_BUTTON_LEFT:	
				leftMouseButtonActive = 1;
				break;
			case SDL_BUTTON_RIGHT:	
				rightMouseButtonActive = 1;
				break;
			case SDL_BUTTON_MIDDLE:	
				middleMouseButtonActive = 1;
				break;
			}
		}

		if (evt.type == SDL_MOUSEBUTTONUP) {
			switch (evt.button.button) {
			case SDL_BUTTON_LEFT:	 
				leftMouseButtonActive = 0;
				break;
			case SDL_BUTTON_RIGHT:	 
				rightMouseButtonActive = 0;
				break;
			case SDL_BUTTON_MIDDLE:	 
				middleMouseButtonActive = 0;
				break;
			}
		}

		// Handle Mouse Motion Events
		if (evt.type == SDL_MOUSEMOTION) {
			if (middleMouseButtonActive) {
				lastXOffset += evt.button.x - mousePressedX;
				lastYOffset += evt.button.y - mousePressedY;
			}
			if (rightMouseButtonActive) {
				lastZOffset += evt.button.x - mousePressedX;
			}
			if (leftMouseButtonActive) {
				navigationRotation[1] += (evt.button.y - mousePressedY) / 100.1;
				navigationRotation[0] += (evt.button.x - mousePressedX) / 100.1;
			}
			mousePressedX = evt.button.x;
			mousePressedY = evt.button.y;
		}

		// ****************************************************************************
		// ****************************************************************************
		// ****************************************************************************
	}
	return true;
}

