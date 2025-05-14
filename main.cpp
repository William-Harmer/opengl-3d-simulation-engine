#include <iostream>
#include <string>
using namespace std;

// OpenGL.
#include "GL/glew.h"
#include "GL/wglew.h"
#pragma comment(lib, "glew32.lib")

// GLM (OpenGL maths library for matrices etc).
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_inverse.hpp"

// Freeglut (For the window).
#include "GL/freeglut.h"                                                                                                                                          

// Library used to load images.
#include "Images/FreeImage.h"

// Creating a shader.
#include "shaders/Shader.h"
CShader* myShader;

// MODEL LOADING --------------------------------------------------------------------------------------------------------------------------------------------
#include "3DStruct/threeDModel.h"
#include "Obj/OBJLoader.h"
COBJLoader objLoader;

// Carts.
CThreeDModel cart1, cart1Top, cart2, cart3, cart4, cart5, cart6, cart7, cart8, cart9, cart10, cart11, cart12, cart13, cart14, cart15, cart16;

// Rotating parts of wheel.
CThreeDModel centerstar;
CThreeDModel wheelringfront1, wheelringfront2;
CThreeDModel innerwheelring1, innerwheelring2;
CThreeDModel wheelline1, wheelline2, wheelline3, wheelline4, wheelline5, wheelline6, wheelline7, wheelline8, wheelline9, wheelline10, wheelline11, 
			 wheelline12, wheelline13, wheelline14, wheelline15, wheelline16, wheelline17, wheelline18, wheelline19, wheelline20, wheelline21, wheelline22, 
			 wheelline23, wheelline24, wheelline25, wheelline26, wheelline27, wheelline28, wheelline29, wheelline30, wheelline31, wheelline32;
CThreeDModel innerrect1, innerrect2, innerrect3, innerrect4, innerrect5, innerrect6, innerrect7, innerrect8, innerrect9, innerrect10, innerrect11, 
			 innerrect12, innerrect13, innerrect14, innerrect15, innerrect16;

// lights.
std::vector<CThreeDModel> lights;

// Static models.
CThreeDModel stand1, stand2, stand3, stand4;
CThreeDModel bottompart;
CThreeDModel centerblock;
CThreeDModel theFloor;
//-----------------------------------------------------------------------------------------------------------------------------------------------------------

// Camera config.
enum CameraMode {
	FREE_CAMERA,
	CART_CAMERA,
	FIXED_CAMERA,
	FIXED_CAMERA_2,
	FIXED_CAMERA_3
};
CameraMode currentCameraMode = FREE_CAMERA;
glm::vec3 cartCamOffset = glm::vec3(-30.0f, -70.0f, 0.0f);

// Free cam config.
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1000.0f); // 3D space pos.
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f); // Looking at pos.
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // Which direction is up.
float cameraSpeed = 0.75f;

// Lighting config.
enum LightMode {
	LIGHT_SOLID = 0,
	LIGHT_BLINK = 1,
	LIGHT_SEQ = 2
};
LightMode currentLightMode = LIGHT_SOLID;
const int LIGHT_BLINK_DURATION = 300;
const int SEQ_LIGHT_COUNT = 5;
const int SEQ_STEP_DURATION = 150;

// Screen settings.
int screenWidth = 1280, screenHeight = 720;

// Mouse config.
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool firstMouse = true;

// Material properties.
float Material_Ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
float Material_Diffuse[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
float Material_Specular[4] = { 0.9f,0.9f,0.8f,1.0f };
float Material_Shininess = 50;

// Light Properties.
float Light_Ambient_And_Diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float Light_Specular[4] = { 1.0f,1.0f,1.0f,1.0f };
float LightPos[4] = { 0.0f, 1.0f, 1.0f, 0.0f };

// Wheel rotation settings.
float wheelRotationAngle = 0.0f;
float wheelRotationSpeed = 0.025f;

// Array that holds key presses.
bool keyState[256] = { false };

// Array that holds the carts.
std::vector<CThreeDModel*> carts;
std::vector<glm::vec3> cartOriginLocations;

// Buffer distance for collision.
const float COLLISION_MARGIN = 3.0f;

// Mouse sens
const float sensitivity = 0.1f;

glm::mat4 ProjectionMatrix;
glm::mat4 ModelViewMatrix;
glm::mat4 view; // Create the view matrix. Will eventually hold the position and orientation of the camera.

// FUNCTIONS ------------------------------------------------------------------------------------------------------------------------------------------------
void init() // Runs once when the program first starts.
{
	// Set the background colour.
	float r = 125.0f / 255.0f;
	float g = 191.0f / 255.0f;
	float b = 221.0f / 255.0f;
	float a = 0.1f;
	glClearColor(r, g, b, a);

	// Enable depth so that depth buffering is used (Tracks how far away each pixel is) and so objects have proper occlusion (Objects that are closer are 
	// drawn first).
	glEnable(GL_DEPTH_TEST);

	// Create a shader for lighting, colour etc.
	myShader = new CShader();
	if (!myShader->CreateShaderProgram("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformations.frag")) cout << 
		"failed to load shader" << endl;

	// Set my shader to be used.
	glUseProgram(myShader->GetProgramObjID());

	// Use 2D texturing. Doesn't seem to do anything for my code.
	glEnable(GL_TEXTURE_2D);

	// Enable hiding of cursor when you are in the window.
	glutSetCursor(GLUT_CURSOR_NONE);

	// Load all the models.
	// InitVBO moves all the model mesh data from the CPU to th GPU.
	cout << " Loading models: " << endl;

	// Wheel rotating models.
	if (objLoader.LoadModel("TestModels/floor.obj")) { theFloor.ConstructModelFromOBJLoader(objLoader); theFloor.InitVBO(myShader); } 
	else cout << " model failed to load floor" << endl;

	if (objLoader.LoadModel("TestModels/Wheelringfront1.obj")) { wheelringfront1.ConstructModelFromOBJLoader(objLoader); wheelringfront1.InitVBO(myShader); }
	else cout << " model failed to load Wheelringfront1" << endl;

	if (objLoader.LoadModel("TestModels/Wheelringfront2.obj")) { wheelringfront2.ConstructModelFromOBJLoader(objLoader); wheelringfront2.InitVBO(myShader); }
	else cout << " model failed to load Wheelringfront2" << endl;

	if (objLoader.LoadModel("TestModels/Centerstar.obj")) { centerstar.ConstructModelFromOBJLoader(objLoader); centerstar.InitVBO(myShader); }
	else cout << " model failed to load Centerstar" << endl;

	if (objLoader.LoadModel("TestModels/Innerwheelring1.obj")) { innerwheelring1.ConstructModelFromOBJLoader(objLoader); innerwheelring1.InitVBO(myShader); }
	else cout << " model failed to load Innerwheelring1" << endl;

	if (objLoader.LoadModel("TestModels/Innerwheelring2.obj")) { innerwheelring2.ConstructModelFromOBJLoader(objLoader); innerwheelring2.InitVBO(myShader); }
	else cout << " model failed to load Innerwheelring2" << endl;

	// Macro.
	// #n makes the number become text.
	// ## concatenates the before and after together.
	#define LOAD_WHEEL_LINE(n) \
        if (objLoader.LoadModel("TestModels/Wheelline" #n ".obj")) { wheelline##n.ConstructModelFromOBJLoader(objLoader); wheelline##n.InitVBO(myShader); } \
        else cout << " model failed to load Wheelline" #n << endl;

		LOAD_WHEEL_LINE(1)  LOAD_WHEEL_LINE(2)  LOAD_WHEEL_LINE(3)  LOAD_WHEEL_LINE(4) LOAD_WHEEL_LINE(5)  LOAD_WHEEL_LINE(6)  LOAD_WHEEL_LINE(7)  
		LOAD_WHEEL_LINE(8) LOAD_WHEEL_LINE(9)  LOAD_WHEEL_LINE(10) LOAD_WHEEL_LINE(11) LOAD_WHEEL_LINE(12) LOAD_WHEEL_LINE(13) LOAD_WHEEL_LINE(14) 
		LOAD_WHEEL_LINE(15) LOAD_WHEEL_LINE(16) LOAD_WHEEL_LINE(17) LOAD_WHEEL_LINE(18) LOAD_WHEEL_LINE(19) LOAD_WHEEL_LINE(20) LOAD_WHEEL_LINE(21) 
		LOAD_WHEEL_LINE(22) LOAD_WHEEL_LINE(23) LOAD_WHEEL_LINE(24) LOAD_WHEEL_LINE(25) LOAD_WHEEL_LINE(26) LOAD_WHEEL_LINE(27) LOAD_WHEEL_LINE(28)
		LOAD_WHEEL_LINE(29) LOAD_WHEEL_LINE(30) LOAD_WHEEL_LINE(31) LOAD_WHEEL_LINE(32)

	#undef LOAD_WHEEL_LINE

	#define LOAD_INNER_RECT(n) \
        if (objLoader.LoadModel("TestModels/Innerrect" #n ".obj")) { innerrect##n.ConstructModelFromOBJLoader(objLoader); innerrect##n.InitVBO(myShader); } \
        else cout << " model failed to load Innerrect" #n << endl;

		LOAD_INNER_RECT(1)  LOAD_INNER_RECT(2)  LOAD_INNER_RECT(3)  LOAD_INNER_RECT(4) LOAD_INNER_RECT(5)  LOAD_INNER_RECT(6)  LOAD_INNER_RECT(7)  
		LOAD_INNER_RECT(8) LOAD_INNER_RECT(9)  LOAD_INNER_RECT(10) LOAD_INNER_RECT(11) LOAD_INNER_RECT(12) LOAD_INNER_RECT(13) LOAD_INNER_RECT(14) 
		LOAD_INNER_RECT(15) LOAD_INNER_RECT(16)

	#undef LOAD_INNER_RECT

	// Static models.
	if (objLoader.LoadModel("TestModels/Bottompart.obj")) { bottompart.ConstructModelFromOBJLoader(objLoader); bottompart.InitVBO(myShader); }
	else cout << " model failed to load Bottompart" << endl;

	if (objLoader.LoadModel("TestModels/Centerblock.obj")) { centerblock.ConstructModelFromOBJLoader(objLoader); centerblock.InitVBO(myShader); }
	else cout << " model failed to load Centerblock" << endl;

	if (objLoader.LoadModel("TestModels/Stand1.obj")) { stand1.ConstructModelFromOBJLoader(objLoader); stand1.InitVBO(myShader); }
	else cout << " model failed to load Stand1" << endl;

	if (objLoader.LoadModel("TestModels/Stand2.obj")) { stand2.ConstructModelFromOBJLoader(objLoader); stand2.InitVBO(myShader); }
	else cout << " model failed to load Stand2" << endl;

	if (objLoader.LoadModel("TestModels/Stand3.obj")) { stand3.ConstructModelFromOBJLoader(objLoader); stand3.InitVBO(myShader); }
	else cout << " model failed to load Stand3" << endl;

	if (objLoader.LoadModel("TestModels/Stand4.obj")) { stand4.ConstructModelFromOBJLoader(objLoader); stand4.InitVBO(myShader); }
	else cout << " model failed to load Stand4" << endl;

	// Light models.
	for (int i = 1; i <= 32; ++i) {
		std::string path = "TestModels/light" + std::to_string(i) + ".obj"; // Get the light location.
		if (objLoader.LoadModel(path.c_str())) { // Load it.
			lights.emplace_back(); // Put it in the array.
			lights.back().ConstructModelFromOBJLoader(objLoader);
			lights.back().InitVBO(myShader);
		}
		else {
			std::cerr << "model failed to load " << path << "\n";
		}
	}

	// Loading the cart models with their coordinates.
	// Cart1. This one is split in half for when you are in the cart.
	if (objLoader.LoadModel("TestModels/cart1.obj")) {
		cart1.ConstructModelFromOBJLoader(objLoader);
		cart1.InitVBO(myShader);
		carts.push_back(&cart1);
		cartOriginLocations.push_back(glm::vec3(0.0f, 776.0f, 0.0f));
	}

	if (objLoader.LoadModel("TestModels/cart1Top.obj")) {
		cart1Top.ConstructModelFromOBJLoader(objLoader);
		cart1Top.InitVBO(myShader);
		carts.push_back(&cart1Top);
		cartOriginLocations.push_back(glm::vec3(0.0f, 776.0f, 0.0f));
	}

	// Cart2.
	if (objLoader.LoadModel("TestModels/cart2.obj")) {
		cart2.ConstructModelFromOBJLoader(objLoader);
		cart2.InitVBO(myShader);

		carts.push_back(&cart2);
		cartOriginLocations.push_back(glm::vec3(-295.0f, 716.0f, 00.0f));
	}

	// Cart3.
	if (objLoader.LoadModel("TestModels/cart3.obj")) {
		cart3.ConstructModelFromOBJLoader(objLoader);
		cart3.InitVBO(myShader);
		carts.push_back(&cart3);
		cartOriginLocations.push_back(glm::vec3(-547.0f, 550.0f, 00.0f));
	}

	// Cart4.
	if (objLoader.LoadModel("TestModels/cart4.obj")) {
		cart4.ConstructModelFromOBJLoader(objLoader);
		cart4.InitVBO(myShader);
		carts.push_back(&cart4);
		cartOriginLocations.push_back(glm::vec3(-716.0f, 297.0f, 0.0f));
	}

	// Cart 5.
	if (objLoader.LoadModel("TestModels/cart5.obj")) {
		cart5.ConstructModelFromOBJLoader(objLoader);
		cart5.InitVBO(myShader);

		carts.push_back(&cart5);
		cartOriginLocations.push_back(glm::vec3(-776.0f, 1.0f, 0.0f));
	}

	// Cart 6.
	if (objLoader.LoadModel("TestModels/cart6.obj")) {
		cart6.ConstructModelFromOBJLoader(objLoader);
		cart6.InitVBO(myShader);
		carts.push_back(&cart6);
		cartOriginLocations.push_back(glm::vec3(-715.0f, -295.0f, 0.0f));
	}

	// Cart 7.
	if (objLoader.LoadModel("TestModels/cart7.obj")) {
		cart7.ConstructModelFromOBJLoader(objLoader);
		cart7.InitVBO(myShader);
		carts.push_back(&cart7);
		cartOriginLocations.push_back(glm::vec3(-547.0f, -548.0f, 0.0f));
	}

	// Cart 8.
	if (objLoader.LoadModel("TestModels/cart8.obj")) {
		cart8.ConstructModelFromOBJLoader(objLoader);
		cart8.InitVBO(myShader);
		carts.push_back(&cart8);
		cartOriginLocations.push_back(glm::vec3(-300.0f, -715.0f, 0.0f));
	}

	// Cart 9.
	if (objLoader.LoadModel("TestModels/cart9.obj")) {
		cart9.ConstructModelFromOBJLoader(objLoader);
		cart9.InitVBO(myShader);
		carts.push_back(&cart9);
		cartOriginLocations.push_back(glm::vec3(0.0f, -776.0f, 0.0f));
	}

	// Cart 10.
	if (objLoader.LoadModel("TestModels/cart10.obj")) {
		cart10.ConstructModelFromOBJLoader(objLoader);
		cart10.InitVBO(myShader);
		carts.push_back(&cart10);
		cartOriginLocations.push_back(glm::vec3(300.0f, -715.0f, 0.0f));
	}

	// Cart 11.
	if (objLoader.LoadModel("TestModels/cart11.obj")) {
		cart11.ConstructModelFromOBJLoader(objLoader);
		cart11.InitVBO(myShader);
		carts.push_back(&cart11);
		cartOriginLocations.push_back(glm::vec3(547.0f, -548.0f, 0.0f));
	}

	// Cart 12.
	if (objLoader.LoadModel("TestModels/cart12.obj")) {
		cart12.ConstructModelFromOBJLoader(objLoader);
		cart12.InitVBO(myShader);
		carts.push_back(&cart12);
		cartOriginLocations.push_back(glm::vec3(715.0f, -295.0f, 0.0f));
	}

	// Cart 13.
	if (objLoader.LoadModel("TestModels/cart13.obj")) {
		cart13.ConstructModelFromOBJLoader(objLoader);
		cart13.InitVBO(myShader);
		carts.push_back(&cart13);
		cartOriginLocations.push_back(glm::vec3(776.0f, 1.0f, 0.0f));
	}

	// Cart 14.
	if (objLoader.LoadModel("TestModels/cart14.obj")) {
		cart14.ConstructModelFromOBJLoader(objLoader);
		cart14.InitVBO(myShader);
		carts.push_back(&cart14);
		cartOriginLocations.push_back(glm::vec3(716.0f, 297.0f, 0.0f));
	}

	// Cart 15.
	if (objLoader.LoadModel("TestModels/cart15.obj")) {
		cart15.ConstructModelFromOBJLoader(objLoader);
		cart15.InitVBO(myShader);
		carts.push_back(&cart15);
		cartOriginLocations.push_back(glm::vec3(547.0f, 550.0f, 0.0f));
	}

	// Cart 16.
	if (objLoader.LoadModel("TestModels/cart16.obj")) {
		cart16.ConstructModelFromOBJLoader(objLoader);
		cart16.InitVBO(myShader);
		carts.push_back(&cart16);
		cartOriginLocations.push_back(glm::vec3(295.0f, 716.0f, 0.0f));
	}
}


void display() // Repeatively runs.
{
	// Clear screen and depth buffer so that when the new values are put in the buffer it doesnt corrupt the data.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Draw any octrees for debugging.
	//wheelringfront1.DrawOctreeLeaves(myShader);
	//centerblock.DrawOctreeLeaves(myShader);

	// Print the camera position for debugging.
	//std::cout << "cameraPos: (" << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << ")\n";

	// Use our shader.
	GLuint prog = myShader->GetProgramObjID();
	glUseProgram(prog);

	// Send projection matrix into the bound shader?
	glUniformMatrix4fv(glGetUniformLocation(prog, "ProjectionMatrix"),1, GL_FALSE,glm::value_ptr(ProjectionMatrix));

	if (currentCameraMode == FREE_CAMERA) {
		view = glm::lookAt(cameraPos, cameraTarget, cameraUp); // These values are adjusted based on the keyboard presses and mouse movement.
	}
	else if (currentCameraMode == FIXED_CAMERA) {
		glm::vec3 pos = glm::vec3(0.0, -895.317f, 1585.21f);
		glm::vec3 lookingAt = glm::vec3(0.0f, -150.0f, 0.0f);
		view = glm::lookAt(pos, lookingAt, cameraUp);
	}
	else if (currentCameraMode == FIXED_CAMERA_2) {
		glm::vec3 pos = glm::vec3(1561.35f, 785.457f, 1831.77f);
		glm::vec3 lookingAt = glm::vec3(0.0f, 0.0f, 0.0f);
		view = glm::lookAt(pos, lookingAt, cameraUp);
	}
	else if (currentCameraMode == FIXED_CAMERA_3) {
		glm::vec3 pos = glm::vec3(-1268.08f, -952.372f, -1726.81f);
		glm::vec3 lookingAt = glm::vec3(0.0f, 0.0f, 0.0f);
		view = glm::lookAt(pos, lookingAt, cameraUp);
	}
	else if (currentCameraMode == CART_CAMERA) {
	
		// Recreate the rotation matrix that the carts do.
		glm::mat4 rotation = glm::rotate(
			glm::mat4(1.0f),					// Start with identity.
			glm::radians(wheelRotationAngle),	// How far to rotate.
			glm::vec3(0.0f, 0.0f, 1.0f)			// Around the z-axis.
		);

		glm::vec3 cartOriginalLocation = cartOriginLocations[0]; // Get cart1's origin location.

		// Apply the rotation to the original position to get the world position.
		glm::vec3 cartWorldPos = glm::vec3(rotation * glm::vec4(cartOriginalLocation, 1.0f));

		// Allow moving around with the mouse.
		// Yaw, left and right. ϕ.
		// Pitch, up and down. θ.
		// Need to convert the yaw and pitch info into a vector.
		// https://math.stackexchange.com/questions/2618527/converting-from-yaw-pitch-roll-to-vector
		glm::vec3 lookAt;
		lookAt.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		lookAt.y = sin(glm::radians(pitch));
		lookAt.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		// Now we have a direction vector.

		glm::vec3 camPos = cartWorldPos + cartCamOffset; // Add the offset so they are on the seat of the cart.
		glm::vec3 camTarget = camPos + lookAt; // Make the direction vector apply to actual coords of where to look at.
		view = glm::lookAt(camPos, camTarget, cameraUp); // Adjust the camera accordingly.
	}

	// Send view matrix from CPU to shader?
	glUniformMatrix4fv(glGetUniformLocation(prog, "ViewMatrix"),1, GL_FALSE,glm::value_ptr(view));

	// Upload light properties from the CPU into shader?
	glUniform4fv(glGetUniformLocation(prog, "LightPos"), 1, LightPos);
	glUniform4fv(glGetUniformLocation(prog, "light_ambient"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(prog, "light_diffuse"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(prog, "light_specular"), 1, Light_Specular);

	// Same for material.
	glUniform4fv(glGetUniformLocation(prog, "material_ambient"), 1, Material_Ambient);
	glUniform4fv(glGetUniformLocation(prog, "material_diffuse"), 1, Material_Diffuse);
	glUniform4fv(glGetUniformLocation(prog, "material_specular"), 1, Material_Specular);
	glUniform1f(glGetUniformLocation(prog, "material_shininess"), Material_Shininess);

	// DRAW STATIC OBJECTS ----------------------------------------------------------------------------------------------------------------------------------
	
	// Can all be done at the same time as their model matrices are the same (identity).
	{
		// Model matrix: Object’s local space into world space.
		// View matrix: Camera world space into camera space. Basically the camera position and where it is looking.
		// Model view matrix: Object world space to camera space. Says where each object is relative to the camera, maybe that object is not even in the shot.
		// Every object needs to be converted into camera space and so every object needs a model view matrix.

		// Make model view matrix = view * model.
		// Only need to times by an identity matrix as the object is static and in the correct position already so we are saying 'apply no extra transform 
		// as it is already in the right place'.
		glm::mat4 mv = view * glm::mat4(1.0f);

		// Get the normal matrix.
		// Needed for correct lighting if the object is non uniform scale I think.
		glm::mat3 nm = glm::inverseTranspose(glm::mat3(mv));

		// Upload to the shader
		glUniformMatrix3fv(glGetUniformLocation(prog, "NormalMatrix"), 1, GL_FALSE, glm::value_ptr(nm));
		glUniformMatrix4fv(glGetUniformLocation(prog, "ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(mv));

		// Now draw the objects
		theFloor.DrawElementsUsingVBO(myShader);
		stand1.DrawElementsUsingVBO(myShader);
		stand2.DrawElementsUsingVBO(myShader);
		stand3.DrawElementsUsingVBO(myShader);
		stand4.DrawElementsUsingVBO(myShader);
		centerblock.DrawElementsUsingVBO(myShader);
		bottompart.DrawElementsUsingVBO(myShader);
	}
	// DRAW WHEEL ROTATING OBJECTS --------------------------------------------------------------------------------------------------------------------------

	// Build the rotation matrix that will be used for all the wheel pieces.
	glm::mat4 rotationMatrix = glm::rotate(
		glm::mat4(1.0f),                  // Start with identity.
		glm::radians(wheelRotationAngle), // How far to rotate.
		glm::vec3(0.0f, 0.0f, 1.0f)       // Around the z-axis.
	);

	// Function that will draw all the rotating parts of the wheel.
	// [&] means you can capture any variables you need into the scope of the function.
	auto drawRot = [&](CThreeDModel& m) {
		// Get the model view matrix = view * model. Remember this time because there is a rotation, we need to times by the model matrix not just by an 
		// identity matrix. The rotation is around the point 0,0,0 so we don't need to do a translation first to change the pivot point.
		glm::mat4 mv = view * rotationMatrix;
		glm::mat3 nm = glm::inverseTranspose(glm::mat3(mv)); // Also get the normal matrix.
		glUniformMatrix3fv(glGetUniformLocation(prog, "NormalMatrix"), 1, GL_FALSE, glm::value_ptr(nm));
		glUniformMatrix4fv(glGetUniformLocation(prog, "ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(mv));
		m.DrawElementsUsingVBO(myShader);
	};

	drawRot(centerstar);
	drawRot(wheelringfront1); drawRot(wheelringfront2);
	drawRot(innerwheelring1); drawRot(innerwheelring2);
	drawRot(wheelline1); drawRot(wheelline2); drawRot(wheelline3); drawRot(wheelline4); drawRot(wheelline5); drawRot(wheelline6); drawRot(wheelline7);
	drawRot(wheelline8); drawRot(wheelline9); drawRot(wheelline10); drawRot(wheelline11);  drawRot(wheelline12); drawRot(wheelline13); drawRot(wheelline14);
	drawRot(wheelline15); drawRot(wheelline16); drawRot(wheelline17); drawRot(wheelline18); drawRot(wheelline19); drawRot(wheelline20); drawRot(wheelline21);
	drawRot(wheelline22); drawRot(wheelline23); drawRot(wheelline24); drawRot(wheelline25); drawRot(wheelline26); drawRot(wheelline27); drawRot(wheelline28);
	drawRot(wheelline29); drawRot(wheelline30); drawRot(wheelline31); drawRot(wheelline32);
	drawRot(innerrect1); drawRot(innerrect2); drawRot(innerrect3); drawRot(innerrect4); drawRot(innerrect5); drawRot(innerrect6); drawRot(innerrect7);
	drawRot(innerrect8); drawRot(innerrect9); drawRot(innerrect10); drawRot(innerrect11); drawRot(innerrect12); drawRot(innerrect13); drawRot(innerrect14); 
	drawRot(innerrect15); drawRot(innerrect16);

	// DRAW LIGHTS ------------------------------------------------------------------------------------------------------------------------------------------
	int elapsed = glutGet(GLUT_ELAPSED_TIME); // Gives you the time in ms since you initialized FreeGLUT.

	// get the slot in which these material properties are stored.
	static GLint emission = glGetUniformLocation(prog, "material_emission");

	// Did try and change the material specular and shininess but didnt make much difference.

	glm::vec4 emitOn(1.0f, 1.0f, 0.2f, 1.0f); // For when the lights are on. RGBA.
	glm::vec4 emitOff(0.0f); // Off is just nothing.

	switch (currentLightMode) {
	case LIGHT_SOLID:
		glUniform4fv(emission, 1, glm::value_ptr(emitOn)); // Set the emission to the emitOn colour.
		
		for (auto& L : lights) // Also add the rotation on each of the lights.
			drawRot(L);

		break;

	case LIGHT_BLINK:
	{
		
		// elapsed / LIGHT_BLINK_DURATION, how many chunks of 300ms have gone by.
		// % 2, if chunks are even 0, if chunks are odd 1.
		bool blink = ((elapsed / LIGHT_BLINK_DURATION) % 2) == 0;

		glUniform4fv(emission, 1, glm::value_ptr(blink ? emitOn : emitOff)); // If blink is 1 turn on, if 0 turn off.

		for (auto& L : lights)
			drawRot(L);

		break;
	}

	case LIGHT_SEQ:
	{
		const int SIDE_COUNT = 16; // How many lights on one side of the wheel.

		// Determine where the head is.
		// elapsed / SEQ_STEP_DURATION, how many chunks of SEQ_STEP_DURATION have passed.
		// % SIDE_COUNT, brings the value in the range 1 - 16.
		int head = (elapsed / SEQ_STEP_DURATION) % SIDE_COUNT;

		// What should the gap be between each light?
		int gap = SIDE_COUNT / SEQ_LIGHT_COUNT;

		for (int i = 0; i < (int)lights.size(); ++i) { // For every light.

			// If we are above light 16 deduct 16.
			// Condition ? valueIfTrue : valueIfFalse.
			int sideIndex = (i < SIDE_COUNT) ? i : (i - SIDE_COUNT); 

			bool on = false;
			// Find out what lights should be on.
			for (int k = 0; k < SEQ_LIGHT_COUNT; ++k) { // For the amount of lights we want on at once.

				// (k * gap), makes each of the lights evenly spaced around the ring.
				// Head is base postion so add (k * gap) so we know what light we should be on.
				// % SIDE_COUNT make sure it is in the range 0,15 so it will loop back if needed.
				int litSlot = (head + (k * gap)) % SIDE_COUNT;

				if (sideIndex == litSlot) { // Is light that should be on the same value as this current light?
					on = true;
					break;
				}
			}

			glUniform4fv(emission, 1, glm::value_ptr(on ? emitOn : emitOff)); // Set light on if on was true.

			drawRot(lights[i]); // Also rotate that light as well.
		}
		break;
	}
	}

	// Restore emission so nothing else is tinted.
	glUniform4fv(emission, 1, glm::value_ptr(emitOff));

	// DRAW CARTS -------------------------------------------------------------------------------------------------------------------------------------------
	for (size_t i = 0; i < carts.size(); ++i) { // For each cart in the array
		CThreeDModel* cart = carts[i]; // Get the cart

		// If we’re in cart camera mode, dont draw the top of cart1.
		if (currentCameraMode == CART_CAMERA && cart == &cart1Top)
			continue;

		const glm::vec3& origin = cartOriginLocations[i]; // Get that carts origin.
		glm::vec3 rotatedPos = glm::vec3(rotationMatrix * glm::vec4(origin, 1.0f)); // Rotate that origin by the rotation matrix to get the new position.
		// Set w to 1 so that translation is also applied?? I think.
		glm::vec3 deltaPos = rotatedPos - origin; // Find the position of where the cart needs to move to.

		glm::mat4 modelCart = glm::translate(glm::mat4(1.0f), deltaPos); // Move the cart to those coordinates.

		// Then make model view matrix and normal matrix.
		glm::mat4 mv = view * modelCart;
		glm::mat3 nm = glm::inverseTranspose(glm::mat3(mv));
		glUniformMatrix3fv(glGetUniformLocation(prog, "NormalMatrix"), 1, GL_FALSE, glm::value_ptr(nm));
		glUniformMatrix4fv(glGetUniformLocation(prog, "ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(mv));

		cart->DrawElementsUsingVBO(myShader); // Draw that cart in that position.
	}

	glFlush();
	glutSwapBuffers(); // Exchange back buffer to front buffer.
}


void reshape(int width, int height) // Resize the window.
{
	screenWidth = width; screenHeight = height;
	glViewport(0, 0, width, height);
	ProjectionMatrix = glm::perspective(glm::radians(60.0f), (GLfloat)screenWidth / (GLfloat)screenHeight, 1.0f, 6000.0f);
}

bool CheckCollision(const glm::vec3& worldPos) {

	// Get the wheel rotation matrix and the inverse.
	glm::mat4 rotM = glm::rotate(glm::mat4(1.0f),glm::radians(wheelRotationAngle),glm::vec3(0, 0, 1));
	glm::mat4 invRotM = glm::inverse(rotM);

	// Test all collisions for things that dont rotate.
	for (auto m : { &theFloor, &bottompart, &centerblock,
					&stand1, &stand2, &stand3, &stand4 }) // For each static model.
	{
		// If the positions being given hit, return true.
		if (m->IsPointInLeaf(worldPos.x, worldPos.y, worldPos.z))
			return true;
	}

	// For wheel rotating objects.
	for (auto m : {
		&wheelringfront1,&wheelringfront2,
		&wheelline1,&wheelline2,&wheelline3,&wheelline4,
		&wheelline5,&wheelline6,&wheelline7,&wheelline8,
		&wheelline9,&wheelline10,&wheelline11,&wheelline12,
		&wheelline13,&wheelline14,&wheelline15,&wheelline16,
		&wheelline17,&wheelline18,&wheelline19,&wheelline20,
		&wheelline21,&wheelline22,&wheelline23,&wheelline24,
		&wheelline25,&wheelline26,&wheelline27,&wheelline28,
		&wheelline29,&wheelline30,&wheelline31,&wheelline32,
		&innerwheelring1,&innerwheelring2,
		&innerrect1,&innerrect2,&innerrect3,&innerrect4,
		&innerrect5,&innerrect6,&innerrect7,&innerrect8,
		&innerrect9,&innerrect10,&innerrect11,&innerrect12,
		&innerrect13,&innerrect14,&innerrect15,&innerrect16,
		&centerstar
		})
	{

		glm::vec3 local = glm::vec3(invRotM * glm::vec4(worldPos, 1.0f)); // Now we have the coordinates of where the point would lie unrotated now.
		
		// Then check
		if (m->IsPointInLeaf(local.x, local.y, local.z))
			return true;
	}

	// For lights same thing.
	for (auto& L : lights) {
		// Undo the rotation of the object currently.
		glm::vec3 local = glm::vec3(invRotM * glm::vec4(worldPos, 1.0f));

		// Then check.
		if (L.IsPointInLeaf(local.x, local.y, local.z))
			return true;
	}


	// Now check the moving carts.
	for (size_t i = 0; i < carts.size(); ++i) { // For each cart.
		CThreeDModel* cart = carts[i]; // get the cart.

		// First find the position of where the cart will be.
		const glm::vec3& origin = cartOriginLocations[i]; // Get the origin location.

		// Get the cart to where it is.
		glm::vec3 rotatedPos = glm::vec3(rotM * glm::vec4(origin, 1.0f));
		glm::vec3 deltaPos = rotatedPos - origin;
		glm::mat4 cartM = glm::translate(glm::mat4(1.0f), deltaPos);  // Move it to that position.

		// Then get the inverse again so we can check the point on its original location before rotation was applied.
		glm::mat4 invCartM = glm::inverse(cartM);
		glm::vec3 local = glm::vec3(invCartM * glm::vec4(worldPos, 1.0f));

		if (cart->IsPointInLeaf(local.x, local.y, local.z))
			return true;
	}

	return false;
}

void keyboard(unsigned char key, int x, int y)
{
	keyState[key] = true;

	// These need to be in here as they are toggle not holds.
	if (key == '1') currentCameraMode = FREE_CAMERA;
	else if (key == '2') currentCameraMode = CART_CAMERA;
	else if (key == '3') currentCameraMode = FIXED_CAMERA;
	else if (key == '4') currentCameraMode = FIXED_CAMERA_2;
	else if (key == '5') currentCameraMode = FIXED_CAMERA_3;

	if (key == 'r') {
		currentLightMode = LightMode((currentLightMode + 1) % 3); // This will cycle 1,2,3,1
	}

}

void keyboardUp(unsigned char key, int x, int y)
{
	keyState[key] = false; // Set the key state to false when released.
}

void special(int key, int x, int y)
{
}

void specialUp(int key, int x, int y)
{
}

void processKeys() {

	// ESC to exit
	if (keyState[27]) exit(0);

	if (keyState['q']) wheelRotationAngle += wheelRotationSpeed; // Rotate to the right.
	if (keyState['e']) wheelRotationAngle -= wheelRotationSpeed; // Rotate to the left.

	if (wheelRotationAngle >= 360.0f) wheelRotationAngle -= 360.0f; // If you try to go beyond 360 go back to 0.
	if (wheelRotationAngle < 0.0f)  wheelRotationAngle += 360.0f; // If you try and go bellow 0 go back to 360.

	// Block any other movements if we are not in freecam mode.
	if (currentCameraMode != FREE_CAMERA)
		return;

	glm::vec3 forwardAndBackwards = cameraTarget - cameraPos; // Get a direction vector for going forward.
	glm::vec3 horizontal = glm::cross(forwardAndBackwards, cameraUp); // get a direction vector for going horizontal. 
	// Cross allows us to have the same direction matrix but perpindicular to forwardAndBackwards.

	glm::vec3 up = cameraUp;

	// Move the camera by a given offset.
	auto tryMove = [&](const glm::vec3& delta) {

		// Get the coordinates for if the camera was to move to position delta.
		glm::vec3 newPos = cameraPos + delta;

		if (!CheckCollision(newPos)) { // If we were to move there and there would be no collision, do it.
			cameraPos = newPos; 
			cameraTarget += delta; // Also must change target otherwise you would be looking a different direction I think.
		}
	};

	// The deltas are just the direction vector * the camera speed.
	if (keyState['w'])   tryMove(forwardAndBackwards * cameraSpeed);
	if (keyState['s'])   tryMove(-forwardAndBackwards * cameraSpeed);
	if (keyState['a'])   tryMove(-horizontal * cameraSpeed);
	if (keyState['d'])   tryMove(horizontal * cameraSpeed);
	if (keyState[32])    tryMove(up * cameraSpeed); // Space
	if (keyState['z'])   tryMove(-up * cameraSpeed);
}

void mouse_callback(int xpos, int ypos) // Xpos and ypos is given to us by glutPassiveMotionFunc.
{

	// First time the camera will be directly in the centre to start.
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	// Get how much you have moved the mouse by.
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	// Add the sensitivity
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	// Apply the change in movement to yaw and pitch.
	yaw += xoffset;
	pitch += yoffset;

	// Don't let the head do a full 360 on the pitch.
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;

	// Use the same 
	// Yaw, left and right. ϕ.
	// Pitch, up and down. θ.
	// Need to convert the yaw and pitch info into a vector.
	// https://math.stackexchange.com/questions/2618527/converting-from-yaw-pitch-roll-to-vector

	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	// Now we have a direction vector.

	// Make the direction vector apply to actual coords of where to look at.
	cameraTarget = direction + cameraPos;

	// After handling movement, reset mouse to center
	glutWarpPointer(screenWidth / 2, screenHeight / 2);
	lastX = screenWidth / 2;
	lastY = screenHeight / 2;
}

void idle()
{
	processKeys();

	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("OpenGL FreeGLUT Example: Obj loading");

	//This initialises glew - it must be called after the window is created.
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cout << " GLEW ERROR" << endl;
	}

	//Check the OpenGL version being used
	int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
	cout << OpenGLVersion[0] << " " << OpenGLVersion[1] << endl;


	//initialise the objects for rendering
	init();

	glutReshapeFunc(reshape);
	//specify which function will be called to refresh the screen.
	glutDisplayFunc(display);
	glutSpecialFunc(special);      // For handling special keys like Shift
	glutSpecialUpFunc(specialUp);  // For handling when special keys are released
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutPassiveMotionFunc(mouse_callback);
	glutIdleFunc(idle);


	//starts the main loop. Program loops and calls callback functions as appropriate.
	glutMainLoop();

	return 0;
}
