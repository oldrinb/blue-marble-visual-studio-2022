/**
 * Author: Oldrin Barbulescu
 * Last modified: Feb 5, 2025
 **/

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "scene/shader/shaderProgram/MainProgram.h"
#include "scene/shader/shaderProgram/Text2dProgram.h"
#include "scene/Scene.h"

#include <assimp/postprocess.h>
#include <assimp/version.h>

#include <IL/il.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

using glm::uvec2;
using glm::vec3;

using std::cerr;
using std::cout;
using std::endl;
using std::exception;
using std::exit;
using std::runtime_error;
using std::size_t;
using std::string;
using std::system;
using std::to_string;



const string kTitle("Blue Marble");
const uvec2 kWindowSize(1200u, 800u);
const float kDeltaTranslation(0.02f), kDeltaRotation(2.0f), kDeltaFieldOfView(5.0f);
const float kKeyRepeatSpeed(3.0f);
const float kMouseCursorSpeed(0.25f);
const float kDefaultFieldOfView = 45.0f;

Scene* pScene = nullptr;
int windowsIsIconified = GLFW_FALSE;
bool mouseButtonIsPressed = false;
string glfwError = "";



// SCENE ###################################################################################
// http://earthobservatory.nasa.gov/Features/BlueMarble/BlueMarble_2002.php
const string kSceneFilePath = "earth"; const string kSceneFileNames[] = { "spheres.obj" };
//###########################################################################################



void render();
void clean();

void resizeCallback(GLFWwindow*, int, int);
void iconifyCallback(GLFWwindow*, int);
void keyboardCallback(GLFWwindow*, int, int, int, int);
void mouseButtonCallback(GLFWwindow*, int, int, int);
void cursorPositionCallback(GLFWwindow*, double, double);
void scrollCallback(GLFWwindow*, double, double);
void errorCallback(int, const char*);

void defaultKeyboardCallback(GLFWwindow*, int, int);
void controlKeyboardCallback(int, int);
void cameraMovementCallback(int, int);

void displayException(const char*);
void checkOpenGlErrors();



void init() {
	glClearColor(0.0f, 0.0f, 0.4f, 1.0f);
	glClearDepth(1.0);

	try {
		::pScene = new Scene(::kWindowSize);
		::pScene->setText2DTexture("font", "DejaVuSansMono_256.DDS");

		// initialize shaders
		//###########################################################################################		
		const string kVersion = "#version 330 core\n";

		string path = "shaders/main";
		
		MainProgram::ProgramMode programMode = MainProgram::ProgramMode::NO_SHADING;
		string directive = "#define NO_SHADING_MODE\n";
		::pScene->addShaderSourceCode(programMode, path, { "structures.glsl", "shading.vert", "scene.vert" }, kVersion + directive,
														 { "structures.glsl", "shading.frag", "scene.frag" }, kVersion + directive);
				
		programMode = MainProgram::ProgramMode::FLAT;
		directive = "#define FLAT_SHADING_MODE\n";
		::pScene->addVertexShaderSourceCode(programMode, path, { "structures.glsl", "shading.glsl" }, kVersion + directive);
		::pScene->addShaderSourceCode(programMode, path, { "structures.glsl", "shading.vert", "scene.vert" }, kVersion + directive,
														 { "structures.glsl", "shading.frag", "scene.frag" }, kVersion + directive);

		programMode = MainProgram::ProgramMode::GOURAUD;
		directive = "#define GOURAUD_SHADING_MODE\n";
		::pScene->addVertexShaderSourceCode(programMode, path, { "structures.glsl", "shading.glsl" }, kVersion + directive);
		::pScene->addShaderSourceCode(programMode, path, { "structures.glsl", "shading.vert", "scene.vert" }, kVersion + directive,
														 { "structures.glsl", "shading.frag", "scene.frag" }, kVersion + directive);

		programMode = MainProgram::ProgramMode::PHONG;
		directive = "#define PHONG_SHADING_MODE\n";		
		::pScene->addFragmentShaderSourceCode(programMode, path, { "structures.glsl", "shading.glsl" }, kVersion + directive);
		::pScene->addShaderSourceCode(programMode, path, { "structures.glsl", "scene.vert" }, kVersion + directive,
														 { "structures.glsl", "shading.frag", "scene.frag" }, kVersion + directive);
				
		programMode = MainProgram::ProgramMode::NORMAL_MAPPING;
		directive = "#define NORMAL_MAPPING_MODE\n";
		::pScene->addFragmentShaderSourceCode(programMode, path, { "structures.glsl", "shading.glsl" }, kVersion + directive);
		::pScene->addShaderSourceCode(programMode, path, { "structures.glsl", "scene.vert" }, kVersion + directive,
														 { "structures.glsl", "shading.frag", "scene.frag" }, kVersion + directive);

		path = "shaders/text2D";
		Text2dProgram::ProgramMode text2dProgramMode = Text2dProgram::ProgramMode::TEXT_2D;
		::pScene->addShaderSourceCode(text2dProgramMode, path, { "text2D.vert" }, kVersion, { "text2D.frag" }, kVersion);

		::pScene->compileShaders();
		
	
		// initialize model
		//###########################################################################################
		unsigned int postProcessSteps =
			aiProcess_JoinIdenticalVertices | // Identifies and joins identical vertex data sets within all imported meshes.
			aiProcess_Triangulate | // Triangulates all faces of all meshes.
			aiProcess_CalcTangentSpace | // Calculates the tangents and bitangents for the imported meshes.
			aiProcess_GenSmoothNormals | // Generates smooth normals for all vertices in the mesh.
			aiProcess_ValidateDataStructure | // Validates the imported scene data structure. This makes sure that all indices
											  // are valid, all animationsand bones are linked correctly, all material
											  // references are correct etc.
			aiProcess_RemoveRedundantMaterials |  // Searches for redundant / unreferenced materials and removes them.
			aiProcess_FixInfacingNormals | // This step tries to determine which meshes have normal vectors that are facing
											  // inwards and inverts them.
			aiProcess_SortByPType | // This step splits meshes with more than one primitive type in homogeneous sub-meshes.
			aiProcess_FindDegenerates | // This step searches all meshes for degenerate primitivesand converts them to proper
										   // lines or points.
			aiProcess_FindInvalidData | // This step searches all meshes for invalid data, such as zeroed normal vectors or
			                            // invalid UV coords and removes / fixes them. This is intended to get rid of some
			                            // common exporter errors.
			aiProcess_OptimizeMeshes;// | // A postprocessing step to reduce the number of meshes.


		unsigned int nModels = sizeof(kSceneFileNames) / sizeof(kSceneFileNames[0u]);
		for (unsigned int i = 0u; i < nModels; i++)
			::pScene->import3DModel("model/" + ::kSceneFilePath, ::kSceneFileNames[i], postProcessSteps);
		::pScene->loadBufferData();

		::pScene->setRotationSpeed(20.0f);

		::pScene->addCamera(vec3(0.6f, 0.0f, 1.3f), vec3(0.0f), ::kDefaultFieldOfView);
		::pScene->addCamera(vec3(1.65f, -1.25f, 1.0f), vec3(0.0f), ::kDefaultFieldOfView);
		::pScene->setLight(vec3(-1.0f, 0.0f, 0.0f), vec3(1.0f), vec3(1.0f), vec3(1.0f, 0.941f, 0.898f));
		::pScene->setRotationSpeed(1.0f);
		::pScene->setMeshRotationSpeed(0, 1, 1.0f);
		::pScene->rotateMesh(-1, -1, -90.0f, vec3(0.0f, 1.0f, 0.0f));
		::pScene->scaleMesh(0, 1, vec3(1.008f));		

		::pScene->initializeSceneParameters();
	}
	catch (const exception& kException) {
		throw runtime_error("main::init > " + string(kException.what()));
	}
	
	checkOpenGlErrors();
}



int main(int, char**) {
	int glfw = GLFW_FALSE;
	int exitStatus = EXIT_FAILURE;
	GLFWwindow* pWindow = nullptr;

	try {
		glfwSetErrorCallback(errorCallback);
		glfw = glfwInit();
		if (glfw != GLFW_TRUE) throw runtime_error("GLFW|Cannot initialize GLFW library.");
		
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

		pWindow = glfwCreateWindow(static_cast<int>(::kWindowSize.x), static_cast<int>(::kWindowSize.y),
			                       ::kTitle.c_str(), NULL, NULL);
		if (!pWindow) throw runtime_error("GLFW|Cannot create GLFW Window.");

		glfwMakeContextCurrent(pWindow);

		glfwSetFramebufferSizeCallback(pWindow, resizeCallback);
		glfwSetWindowIconifyCallback(pWindow, iconifyCallback);
		glfwSetKeyCallback(pWindow, keyboardCallback);
		glfwSetMouseButtonCallback(pWindow, mouseButtonCallback);
		glfwSetCursorPosCallback(pWindow, cursorPositionCallback);
		glfwSetScrollCallback(pWindow, scrollCallback);
		
		int gl3w = gl3wInit();
		if (gl3w != GL3W_OK)
			throw runtime_error("GL3W|Failed to initialize OpenGL.");
		if (gl3wIsSupported(4, 6) == 0) throw runtime_error("GL3W|OpenGL 4.6 is not supported.");

		glfwSwapInterval(1);

		ilInit();

		cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
		cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
		cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
		cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl << endl;
		cout << "GLFW version: " << glfwGetVersionString() << endl;
		cout << "DevIL version: " << ilGetInteger(IL_VERSION_NUM) << endl;
		cout << "Assimp version: " << aiGetVersionMajor() << "." << aiGetVersionMinor() << endl;
		cout << "GLM version: " << GLM_VERSION << endl;
		cout << endl;

		glEnable(GL_MULTISAMPLE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		GLint dimensions[2u];
		glGetIntegerv(GL_MAX_VIEWPORT_DIMS, dimensions);
		if (::kWindowSize.x > static_cast<unsigned int>(dimensions[0u]) || 
			::kWindowSize.y > static_cast<unsigned int>(dimensions[1u]))
			throw runtime_error("main::main|Maximum viewport dimensions exceeded.");

		init();

		while (!glfwWindowShouldClose(pWindow)) {
			if (::windowsIsIconified == GLFW_FALSE) {
				render();
				glfwSwapBuffers(pWindow);				
			}
			glfwPollEvents();
		}

		exitStatus = EXIT_SUCCESS;
	}
	catch (const exception& kException) {
		displayException(kException.what());
	}

	clean();

	if (glfw == GLFW_TRUE) {
		if (pWindow) glfwDestroyWindow(pWindow);
		glfwTerminate();
	}

	if (exitStatus == EXIT_FAILURE) {
		cerr << endl << endl << "EXIT FAILURE" << endl << endl;
		system("PAUSE");
		exit (EXIT_FAILURE);
	}

	//cout << endl << endl << "EXIT SUCCESS" << endl << endl;
	//system("PAUSE");
	return EXIT_SUCCESS;
}



void render() {
	static unsigned int sFps = 0u, sFrameCount = 0u;
	static double sLastTime = 0.0, sLastFrameTime = 0.0;

	float deltaTime = static_cast<float>(glfwGetTime() - sLastTime);

	sFrameCount++;
	if (glfwGetTime() - sLastFrameTime >= 1.0) {
		sFps = sFrameCount;
		sFrameCount = 0u;
		sLastFrameTime = glfwGetTime();
	}

	sLastTime = glfwGetTime();

	try {
		::pScene->updateRotation(deltaTime);
		::pScene->render(glfwGetTime(), sFps);
	}
	catch (const exception& kException) {
		throw runtime_error("main::render > " + string(kException.what()));
	}	

	//checkOpenGlErrors();
}



void clean() {
	if (::pScene) delete ::pScene;
}



void resizeCallback(GLFWwindow* pWindow, int width, int height) {
	if (::windowsIsIconified == GLFW_FALSE) {
		if (width > 0 && height > 0) {
			::pScene->setWindowSize(uvec2(width, height));
			glfwSwapBuffers(pWindow);
		}
		else if (width > 0) glfwSetWindowSize(pWindow, width, 1);
		else if (height > 0) glfwSetWindowSize(pWindow, 1, height);
		else glfwSetWindowSize(pWindow, 1, 1);
	}
}



void iconifyCallback(GLFWwindow*, int iconified) {
	::windowsIsIconified = iconified;
}



void keyboardCallback(GLFWwindow* pWindow, int key, int, int action, int mods) {
	try {
		switch (mods) {
		default:
			defaultKeyboardCallback(pWindow, key, action);
			break;

		case GLFW_MOD_CONTROL:
			controlKeyboardCallback(key, action);
		}
	}
	catch (const exception& kException) {
		throw runtime_error("main::keyboardCallback > " + string(kException.what()));
	}
}



void mouseButtonCallback(GLFWwindow*, int button, int action, int) {
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		switch (action) {
		case GLFW_PRESS:
			::mouseButtonIsPressed = true;
			break;
		case GLFW_RELEASE:
			::mouseButtonIsPressed = false;
		}
		break;

	case GLFW_MOUSE_BUTTON_MIDDLE:
		switch (action) {
		case GLFW_PRESS:
			try {
				::pScene->resetCameraFieldOfView();
			}
			catch (const exception& kException) {
				throw runtime_error("main::mouseButtonCallback > " + string(kException.what()));
			}
		}
	}	
}



void cursorPositionCallback(GLFWwindow*, double xpos, double ypos) {
	static double sMouseLastCursorPositionX = 0.0, sMouseLastCursorPositionY = 0.0;

	if (::mouseButtonIsPressed) {
		float angleX = ::kMouseCursorSpeed * static_cast<float>(ypos - sMouseLastCursorPositionY);
		float angleY = ::kMouseCursorSpeed * static_cast<float>(xpos - sMouseLastCursorPositionX);

		::pScene->updateMouseMovement(angleX, angleY);
	}

	sMouseLastCursorPositionX = xpos;
	sMouseLastCursorPositionY = ypos;
}



void scrollCallback(GLFWwindow*, double, double yoffset) {
	try {
		if (yoffset > 0.0) ::pScene->changeCameraFieldOfView(::kDeltaFieldOfView);
		else if (yoffset < 0.0) ::pScene->changeCameraFieldOfView(-::kDeltaFieldOfView);
	}
	catch (const exception& kException) {
		throw runtime_error("main::scrollCallback > " + string(kException.what()));
	}
}



void errorCallback(int errorCode, const char* pkDescription) {
	::glfwError = string(pkDescription) + " (error " + to_string (errorCode) + ")";
}



void defaultKeyboardCallback(GLFWwindow* pWindow, int key, int action) {
	try {
		switch (action) {
		case GLFW_PRESS:
			switch (key) {
			case GLFW_KEY_SPACE: ::pScene->toggleRotation(glfwGetTime());
				break;
			case GLFW_KEY_0: ::pScene->setShadingModel(Scene::ShadingModel::PHONG);
				break;
			case GLFW_KEY_7: ::pScene->setShadingModel(Scene::ShadingModel::NO_SHADING);
				break;
			case GLFW_KEY_8: ::pScene->setShadingModel(Scene::ShadingModel::FLAT);
				break;
			case GLFW_KEY_9: ::pScene->setShadingModel(Scene::ShadingModel::GOURAUD);
				break;
			case GLFW_KEY_A: ::pScene->toggleAmbientLight();
				break;
			case GLFW_KEY_D: ::pScene->toggleDiffuseLight();
				break;
			case GLFW_KEY_F: ::pScene->toggleEmissiveLight();
				break;
			case GLFW_KEY_I: ::pScene->toggleDisplayInfo();
				break;
			case GLFW_KEY_N: ::pScene->toggleNormalMapping();
				break;
			case GLFW_KEY_R: ::pScene->resetRotation(glfwGetTime());
				break;
			case GLFW_KEY_S: ::pScene->toggleSpecularLight();
				break;
			case GLFW_KEY_W: ::pScene->toggleWireframe();
				break;
			case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(pWindow, GL_TRUE);
				break;
			case GLFW_KEY_KP_5: ::pScene->resetCamera();
			}
		}

		cameraMovementCallback(key, action);
	}
	catch (const exception& kException) {
		throw runtime_error("main::defaultKeyboardCallback > " + string(kException.what()));
	}
}



void controlKeyboardCallback(int key, int action) {
	try {
		switch (action) {
		case GLFW_PRESS:
			switch (key) {
			case GLFW_KEY_1: ::pScene->setActiveCamera(0u);
				break;
			case GLFW_KEY_2: ::pScene->setActiveCamera(1u);
				break;
			case GLFW_KEY_3: ::pScene->setActiveCamera(2u);
				break;
			case GLFW_KEY_4: ::pScene->setActiveCamera(3u);
				break;
			case GLFW_KEY_5: ::pScene->setActiveCamera(4u);
			}
		}
	}
	catch (const exception& kException) {
		throw runtime_error("main::controlKeyboardCallback > " + string(kException.what()));
	}
}



void cameraMovementCallback(int key, int action) {
	try {
		float speed = 1.0f;

		switch (action) {		
		case GLFW_REPEAT:
			speed *= ::kKeyRepeatSpeed;
		case GLFW_PRESS:
			switch (key) {
			case GLFW_KEY_MINUS:
			case GLFW_KEY_LEFT:
			case GLFW_KEY_DOWN:
			case GLFW_KEY_KP_2:
			case GLFW_KEY_KP_4:
				speed = -speed;
			}

			switch (key) {
			case GLFW_KEY_MINUS:
			case GLFW_KEY_EQUAL: ::pScene->translateCameraForward(speed * ::kDeltaTranslation);
				break;
			case GLFW_KEY_RIGHT:
			case GLFW_KEY_LEFT: ::pScene->translateCameraRight(speed * ::kDeltaTranslation);
				break;
			case GLFW_KEY_DOWN:
			case GLFW_KEY_UP: ::pScene->translateCameraUp(speed * ::kDeltaTranslation);
				break;
			case GLFW_KEY_KP_2:
			case GLFW_KEY_KP_8: ::pScene->rotateCameraUp(speed * ::kDeltaRotation);
				break;
			case GLFW_KEY_KP_4:
			case GLFW_KEY_KP_6: ::pScene->rotateCameraRight(speed * ::kDeltaRotation);
			}
		}
	}
	catch (const exception& kException) {
		throw runtime_error("main::cameraMovementCallback > " + string(kException.what()));
	}
}



void displayException(const char* pkException) {
	const unsigned int kLineLength = 96u;
	const char kSeparator = '|';

	string exception(pkException);
	size_t pos = exception.find(kSeparator);

	if (pos != string::npos) {
		string temp = ":\n" + exception.substr(pos + 1u, exception.length() - pos - 1u);
		if (pos > 0u) temp = " (" + exception.substr(0u, pos) + ")" + temp;
		exception = "Error" + temp;
	}
	else exception = "Error:\n" + exception;

	if (!::glfwError.empty())
		exception += "\n" + ::glfwError;

	cerr << endl << endl << string(kLineLength, '!') << endl;

	pos = 0u;
	while (!exception.empty() && pos != string::npos) {
		pos = exception.find('\n');

		if (pos == string::npos && exception.length() <= kLineLength)
			cerr << exception << endl;		

		else if (pos > kLineLength) {
			cerr << exception.substr(0u, kLineLength) << endl;
			exception = exception.substr(kLineLength);
			pos = 0u;
		}	
		else {
			if (pos > 0u) cerr << exception.substr(0u, pos) << endl;
			exception = exception.substr(pos + 1u);
			pos = 0u;
		}
	}

	cerr << string(kLineLength, '!') << endl << endl;
}


 
void checkOpenGlErrors() {
	GLenum errorCode = glGetError();
	string description = "";
	if (errorCode != GL_NO_ERROR) {
		switch (errorCode) {
		case GL_INVALID_ENUM: description = "Invalid enum.";
			break;
		case GL_INVALID_VALUE: description = "Invalid value.";
			break;
		case GL_INVALID_OPERATION: description = "Invalid operation.";
			break;
		case GL_STACK_OVERFLOW: description = "Stack overflow.";
			break;
		case GL_STACK_UNDERFLOW: description = "Stack underflow.";
			break;
		case GL_OUT_OF_MEMORY: description = "Out of memory.";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: description = "Invalid framebuffer operation.";
			break;
		default: description =  "Unknown error";
		}

		description += " (error " + to_string(errorCode) + ")";
	}

	while ((errorCode = glGetError()) != GL_NO_ERROR) {  }

	if (!description.empty())
		throw runtime_error("OpenGL|" + description);	
}
