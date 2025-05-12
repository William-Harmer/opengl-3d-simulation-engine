#include <iostream>
#include <string>
using namespace std;

//--- OpenGL ---
#include "GL/glew.h"
#include "GL/wglew.h"
#pragma comment(lib, "glew32.lib")
//--------------

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_inverse.hpp"

#include "GL/freeglut.h"
#include "Images/FreeImage.h"
#include "shaders/Shader.h"

CShader* myShader;

// MODEL LOADING
#include "3DStruct/threeDModel.h"
#include "Obj/OBJLoader.h"

// Cart models
CThreeDModel cart1, cart1Top, cart2, cart3, cart4, cart5, cart6, cart7, cart8, cart9, cart10, cart11, cart12, cart13, cart14, cart15, cart16;

// Rotating models
CThreeDModel centerstar;

// Static models
CThreeDModel theFloor;
// Individual new models
CThreeDModel bottompart, centerblock;
CThreeDModel wheelringfront1, wheelringfront2;
CThreeDModel wheelline1, wheelline2, wheelline3, wheelline4, wheelline5, wheelline6, wheelline7, wheelline8;
CThreeDModel wheelline9, wheelline10, wheelline11, wheelline12, wheelline13, wheelline14, wheelline15, wheelline16;
CThreeDModel wheelline17, wheelline18, wheelline19, wheelline20, wheelline21, wheelline22, wheelline23, wheelline24;
CThreeDModel wheelline25, wheelline26, wheelline27, wheelline28, wheelline29, wheelline30, wheelline31, wheelline32;
CThreeDModel innerwheelring1, innerwheelring2;
CThreeDModel innerrect1, innerrect2, innerrect3, innerrect4, innerrect5, innerrect6, innerrect7, innerrect8;
CThreeDModel innerrect9, innerrect10, innerrect11, innerrect12, innerrect13, innerrect14, innerrect15, innerrect16;
CThreeDModel stand1, stand2, stand3, stand4;

// lights
std::vector<CThreeDModel> lights;

COBJLoader objLoader;

float amount = 0;
float temp = 0.002f;

enum CameraMode {
	FREE_CAMERA,
	CART_CAMERA,
	FIXED_CAMERA,
	FIXED_CAMERA_2,
	FIXED_CAMERA_3
};

CameraMode currentCameraMode = FREE_CAMERA;

///END MODEL LOADING


// Make sure to add the octrees so that they can see the problem I was having

glm::mat4 ProjectionMatrix; // matrix for the orthographic projection
glm::mat4 ModelViewMatrix;  // matrix for the modelling and viewing

//glm::mat4 objectRotation;
//glm::vec3 translation = glm::vec3(0.0, 0.0, 0.0);
//glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f); //vector for the position of the object.

//Material properties
float Material_Ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
float Material_Diffuse[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
float Material_Specular[4] = { 0.9f,0.9f,0.8f,1.0f };
float Material_Shininess = 50;

//Light Properties
float Light_Ambient_And_Diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float Light_Specular[4] = { 1.0f,1.0f,1.0f,1.0f };
float LightPos[4] = { 0.0f, 1.0f, 1.0f, 0.0f };

int screenWidth = 1280, screenHeight = 720;

bool keyState[256] = { false };  // Array to store the state of each key (256 keys)


//OPENGL FUNCTION PROTOTYPES
void display();				//called in winmain to draw everything to the screen
void reshape(int width, int height);				//called when the window is resized
void init();				//called in winmain when the program starts.
void processKeys();         //called in winmain to process keyboard input
void idle();		//idle function

// The camera matrix (viewingMatrix called inside display())
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1000.0f); // The inital position of the camera in the 3D space
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f); // The inital look at coordinates
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // Defines the up direction of the camera. It tells the camera what direction is "up" relative to the world coordinates.

glm::vec3 cartTopPos = glm::vec3(000.0f, 776.0f, 0.0f);



// Mouse control variables
float yaw = -90.0f;  // Horizontal angle (initialized facing -Z)
float pitch = 0.0f;  // Vertical angle
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool firstMouse = true;

float cameraSpeed = 0.75f; // You can adjust the movement speed

float wheelRotationAngle = 0.0f;
float wheelRotationSpeed = 0.025f;


std::vector<CThreeDModel*> carts;
std::vector<glm::vec3> cartOffsets;

glm::vec3 cartCamOffset = glm::vec3(-30.0f, -70.0f, 0.0f);

// ?? Light?mode state ????????????????????????????????
enum LightMode {
	LIGHT_SOLID = 0,
	LIGHT_BLINK = 1,
	LIGHT_SEQ = 2
};
LightMode currentLightMode = LIGHT_SOLID;  // start in blinking mode
// —— Sequential chase settings ——
// How many lights to light up at once:
const int SEQ_LIGHT_COUNT = 5;
// How many milliseconds between each step:
const int SEQ_STEP_DURATION = 150;



/*************    START OF OPENGL FUNCTIONS   ****************/
void display()
{
	 //std::cout 
  //      << "cameraPos: (" 
  //      << cameraPos.x << ", " 
  //      << cameraPos.y << ", " 
  //      << cameraPos.z << ")\n";


	// Clear screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader
	GLuint prog = myShader->GetProgramObjID();
	glUseProgram(prog);

	// --- displacement uniform (if still used) ---
	amount += temp;
	if (amount > 1.0f || amount < -1.5f) temp = -temp;
	glUniform1f(glGetUniformLocation(prog, "displacement"), amount);

	// --- Projection matrix ---
	glUniformMatrix4fv(
		glGetUniformLocation(prog, "ProjectionMatrix"),
		1, GL_FALSE,
		glm::value_ptr(ProjectionMatrix)
	);

	// --- View matrix (camera) ---
	glm::mat4 view;

	if (currentCameraMode == FREE_CAMERA) {
		view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
	}
	else if (currentCameraMode == FIXED_CAMERA) {
		glm::vec3 fixedPos = glm::vec3(2000.0f, 20.0f, 2000.0f);
		glm::vec3 fixedTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		view = glm::lookAt(fixedPos, fixedTarget, cameraUp);
	}
	else if (currentCameraMode == FIXED_CAMERA_2) {
		glm::vec3 fixedPos = glm::vec3(2000.0f, 100.0f, 2000.0f);
		glm::vec3 fixedTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		view = glm::lookAt(fixedPos, fixedTarget, cameraUp);
	}
	else if (currentCameraMode == FIXED_CAMERA_3) {
		glm::vec3 fixedPos = glm::vec3(2000.0f, 2000.0f, 2000.0f);
		glm::vec3 fixedTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		view = glm::lookAt(fixedPos, fixedTarget, cameraUp);
	}
	else if (currentCameraMode == CART_CAMERA) {
		// --- 1) recompute the wheel rotation used for all carts ---
		glm::mat4 rotM = glm::rotate(
			glm::mat4(1.0f),
			glm::radians(wheelRotationAngle),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);

		// --- 2) get cart1’s world position via its baked-in offset ---
		glm::vec3 bakedOffset = cartOffsets[0];
		glm::vec3 cartWorldPos = glm::vec3(rotM * glm::vec4(bakedOffset, 1.0f));

		// --- 3) compute look-direction from yaw/pitch (same math as mouse) ---
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(front);

		// --- 4) camera sits at cart + offset, looking out along front ---
		glm::vec3 camPos = cartWorldPos + cartCamOffset;
		glm::vec3 camTarget = camPos + front;
		view = glm::lookAt(camPos, camTarget, cameraUp);
	}


	glUniformMatrix4fv(
		glGetUniformLocation(prog, "ViewMatrix"),
		1, GL_FALSE,
		glm::value_ptr(view)
	);

	// --- Lights ---
	glUniform4fv(glGetUniformLocation(prog, "LightPos"), 1, LightPos);
	glUniform4fv(glGetUniformLocation(prog, "light_ambient"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(prog, "light_diffuse"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(prog, "light_specular"), 1, Light_Specular);

	// --- Material ---
	glUniform4fv(glGetUniformLocation(prog, "material_ambient"), 1, Material_Ambient);
	glUniform4fv(glGetUniformLocation(prog, "material_diffuse"), 1, Material_Diffuse);
	glUniform4fv(glGetUniformLocation(prog, "material_specular"), 1, Material_Specular);
	glUniform1f(glGetUniformLocation(prog, "material_shininess"), Material_Shininess);

	// --- Draw floor ---
	{
		glm::mat4 mv = view * glm::mat4(1.0f);
		glm::mat3 nm = glm::inverseTranspose(glm::mat3(mv));
		glUniformMatrix3fv(glGetUniformLocation(prog, "NormalMatrix"), 1, GL_FALSE, glm::value_ptr(nm));
		glUniformMatrix4fv(glGetUniformLocation(prog, "ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(mv));
		theFloor.DrawElementsUsingVBO(myShader);
	}

	// --- Build a single wheel?rotation matrix ---
	glm::mat4 rotationMatrix = glm::rotate(
		glm::mat4(1.0f),
		glm::radians(wheelRotationAngle),
		glm::vec3(0.0f, 0.0f, 1.0f)
	);

	// --- Helper to draw any part with that rotation ---
	auto drawRot = [&](CThreeDModel& m) {
		glm::mat4 mv = view * rotationMatrix;
		glm::mat3 nm = glm::inverseTranspose(glm::mat3(mv));
		glUniformMatrix3fv(glGetUniformLocation(prog, "NormalMatrix"), 1, GL_FALSE, glm::value_ptr(nm));
		glUniformMatrix4fv(glGetUniformLocation(prog, "ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(mv));
		m.DrawElementsUsingVBO(myShader);
		};

	// --- Draw each rotating piece individually ---
	drawRot(wheelringfront1);
	drawRot(wheelringfront2);
	drawRot(wheelline1);   drawRot(wheelline2);   drawRot(wheelline3);   drawRot(wheelline4);
	drawRot(wheelline5);   drawRot(wheelline6);   drawRot(wheelline7);   drawRot(wheelline8);
	drawRot(wheelline9);   drawRot(wheelline10);  drawRot(wheelline11);  drawRot(wheelline12);
	drawRot(wheelline13);  drawRot(wheelline14);  drawRot(wheelline15);  drawRot(wheelline16);
	drawRot(wheelline17);  drawRot(wheelline18);  drawRot(wheelline19);  drawRot(wheelline20);
	drawRot(wheelline21);  drawRot(wheelline22);  drawRot(wheelline23);  drawRot(wheelline24);
	drawRot(wheelline25);  drawRot(wheelline26);  drawRot(wheelline27);  drawRot(wheelline28);
	drawRot(wheelline29);  drawRot(wheelline30);  drawRot(wheelline31);  drawRot(wheelline32);
	drawRot(innerwheelring1);
	drawRot(innerwheelring2);
	drawRot(innerrect1);   drawRot(innerrect2);   drawRot(innerrect3);   drawRot(innerrect4);
	drawRot(innerrect5);   drawRot(innerrect6);   drawRot(innerrect7);   drawRot(innerrect8);
	drawRot(innerrect9);   drawRot(innerrect10);  drawRot(innerrect11);  drawRot(innerrect12);
	drawRot(innerrect13);  drawRot(innerrect14);  drawRot(innerrect15);  drawRot(innerrect16);
	drawRot(centerstar);



	//centerstar.DrawOctreeLeaves(myShader);
	//wheelringfront1.DrawOctreeLeaves(myShader);
	//wheelringfront1.DrawOctreeLeaves(myShader);
	//wheelringfront2.DrawOctreeLeaves(myShader);
	//innerwheelring1.DrawOctreeLeaves(myShader);

	// --- Draw bottompart (static) ---
	{
		glm::mat4 mv = view * glm::mat4(1.0f);
		glm::mat3 nm = glm::inverseTranspose(glm::mat3(mv));
		glUniformMatrix3fv(
			glGetUniformLocation(prog, "NormalMatrix"),
			1, GL_FALSE,
			glm::value_ptr(nm)
		);
		glUniformMatrix4fv(
			glGetUniformLocation(prog, "ModelViewMatrix"),
			1, GL_FALSE,
			glm::value_ptr(mv)
		);
		bottompart.DrawElementsUsingVBO(myShader);
	}

	// --- Draw centerblock (static) ---
	{
		glm::mat4 mv = view * glm::mat4(1.0f);
		glm::mat3 nm = glm::inverseTranspose(glm::mat3(mv));
		glUniformMatrix3fv(
			glGetUniformLocation(prog, "NormalMatrix"),
			1, GL_FALSE,
			glm::value_ptr(nm)
		);
		glUniformMatrix4fv(
			glGetUniformLocation(prog, "ModelViewMatrix"),
			1, GL_FALSE,
			glm::value_ptr(mv)
		);
		centerblock.DrawElementsUsingVBO(myShader);
		//centerblock.DrawOctreeLeaves(myShader);
	}

	// --- Draw stands 1–4 (static) ---
	{
		glm::mat4 mv = view * glm::mat4(1.0f);
		glm::mat3 nm = glm::inverseTranspose(glm::mat3(mv));
		glUniformMatrix3fv(
			glGetUniformLocation(prog, "NormalMatrix"),
			1, GL_FALSE,
			glm::value_ptr(nm)
		);
		glUniformMatrix4fv(
			glGetUniformLocation(prog, "ModelViewMatrix"),
			1, GL_FALSE,
			glm::value_ptr(mv)
		);
		stand1.DrawElementsUsingVBO(myShader);
		stand2.DrawElementsUsingVBO(myShader);
		stand3.DrawElementsUsingVBO(myShader);
		stand4.DrawElementsUsingVBO(myShader);
	}

	// ——— Blinking glow for all lights ———
		// ——— Light glow for all lights ———
	int elapsed = glutGet(GLUT_ELAPSED_TIME);
	static GLint locEmit = glGetUniformLocation(prog, "material_emission");
	static GLint locSpecular = glGetUniformLocation(prog, "material_specular");
	static GLint locShininess = glGetUniformLocation(prog, "material_shininess");
	glm::vec4 emitOn(1.0f, 1.0f, 0.2f, 1.0f), emitOff(0.0f);

	switch (currentLightMode) {
	case LIGHT_SOLID:
		// All lights on
		glUniform4fv(locEmit, 1, glm::value_ptr(emitOn));
		for (auto& L : lights)
			drawRot(L);
		break;

	case LIGHT_BLINK:
	{
		// All lights toggle on/off every 300ms
		bool blink = ((elapsed / 300) % 2) == 0;
		glUniform4fv(locEmit, 1, glm::value_ptr(blink ? emitOn : emitOff));
		for (auto& L : lights)
			drawRot(L);
		break;
	}

	case LIGHT_SEQ:
	{
		const int SIDE_COUNT = 16;
		// which “head” we’re at this frame
		int step = (elapsed / SEQ_STEP_DURATION) % SIDE_COUNT;
		// how far apart each of the SEQ_LIGHT_COUNT lights should be
		int offset = SIDE_COUNT / SEQ_LIGHT_COUNT; // 16/3 == 5

		for (int i = 0; i < (int)lights.size(); ++i) {
			// map [0..lights.size()) ? [0..15] per side
			int sideIndex = (i < SIDE_COUNT) ? i : (i - SIDE_COUNT);

			// turn on if this index matches any of the 3 evenly?spaced positions
			bool on = false;
			for (int k = 0; k < SEQ_LIGHT_COUNT; ++k) {
				if (sideIndex == (step + k * offset) % SIDE_COUNT) {
					on = true;
					break;
				}
			}

			glUniform4fv(locEmit, 1, glm::value_ptr(on ? emitOn : emitOff));
			drawRot(lights[i]);
		}
		break;
	}


	}

	// restore specular & shininess so scene isn’t tinted
	glUniform4fv(locEmit, 1, glm::value_ptr(emitOff));
	glUniform4fv(locSpecular, 1, Material_Specular);
	glUniform1f(locShininess, Material_Shininess);


	// ——— end blinking glow ———




	for (size_t i = 0; i < carts.size(); ++i) {
		CThreeDModel* cart = carts[i];

		// if we’re in cart camera mode, skip the top of cart1
		if (currentCameraMode == CART_CAMERA && cart == &cart1Top)
			continue;

		const glm::vec3& offset = cartOffsets[i];
		glm::vec3 rotatedPos = glm::vec3(rotationMatrix * glm::vec4(offset, 1.0f));
		glm::vec3 deltaPos = rotatedPos - offset;

		glm::mat4 modelCart = glm::translate(glm::mat4(1.0f), deltaPos);
		glm::mat4 mv = view * modelCart;
		glm::mat3 nm = glm::inverseTranspose(glm::mat3(mv));
		glUniformMatrix3fv(
			glGetUniformLocation(prog, "NormalMatrix"), 1, GL_FALSE, glm::value_ptr(nm));
		glUniformMatrix4fv(
			glGetUniformLocation(prog, "ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(mv));

		cart->DrawElementsUsingVBO(myShader);
	}

	// Flush and swap
	glFlush();
	glutSwapBuffers();
}

void reshape(int width, int height)		// Resize the OpenGL window
{
	screenWidth = width; screenHeight = height;           // to ensure the mouse coordinates match 
	// we will use these values to set the coordinate system

	glViewport(0, 0, width, height);						// Reset The Current Viewport

	//Set the projection matrix
	ProjectionMatrix = glm::perspective(glm::radians(60.0f), (GLfloat)screenWidth / (GLfloat)screenHeight, 1.0f, 6000.0f);
}

void init()
{

	float r = 125.0f / 255.0f;  // ? 0.137f
	float g = 191.0f / 255.0f;  // ? 0.663f
	float b = 221.0f / 255.0f;  // ? 0.839f
	float a = 0.1f;             // same alpha

	glClearColor(r, g, b, a);

	glEnable(GL_DEPTH_TEST);

	myShader = new CShader();
	if (!myShader->CreateShaderProgram("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformations.frag"))
		cout << "failed to load shader" << endl;

	glUseProgram(myShader->GetProgramObjID());
	glEnable(GL_TEXTURE_2D);

	cout << " loading models " << endl;

	// Floor
	if (objLoader.LoadModel("TestModels/floor.obj")) { theFloor.ConstructModelFromOBJLoader(objLoader); theFloor.InitVBO(myShader); }
	else cout << " model failed to load floor" << endl;
	// Centerstar
	if (objLoader.LoadModel("TestModels/Centerstar.obj")) { centerstar.ConstructModelFromOBJLoader(objLoader); centerstar.InitVBO(myShader); }
	else cout << " model failed to load Centerstar" << endl;
	// Bottompart
	if (objLoader.LoadModel("TestModels/Bottompart.obj")) { bottompart.ConstructModelFromOBJLoader(objLoader); bottompart.InitVBO(myShader); }
	else cout << " model failed to load Bottompart" << endl;
	// Centerblock
	if (objLoader.LoadModel("TestModels/Centerblock.obj")) { centerblock.ConstructModelFromOBJLoader(objLoader); centerblock.InitVBO(myShader); }
	else cout << " model failed to load Centerblock" << endl;

	// Wheelringfront1
	if (objLoader.LoadModel("TestModels/Wheelringfront1.obj")) { wheelringfront1.ConstructModelFromOBJLoader(objLoader); wheelringfront1.InitVBO(myShader); }
	else cout << " model failed to load Wheelringfront1" << endl;
	// Wheelringfront2
	if (objLoader.LoadModel("TestModels/Wheelringfront2.obj")) { wheelringfront2.ConstructModelFromOBJLoader(objLoader); wheelringfront2.InitVBO(myShader); }
	else cout << " model failed to load Wheelringfront2" << endl;

	// Wheellines 1-32
#define LOAD_WHEEL_LINE(n) \
        if (objLoader.LoadModel("TestModels/Wheelline" #n ".obj")) { wheelline##n.ConstructModelFromOBJLoader(objLoader); wheelline##n.InitVBO(myShader); } \
        else cout << " model failed to load Wheelline" #n << endl;
		LOAD_WHEEL_LINE(1)  LOAD_WHEEL_LINE(2)  LOAD_WHEEL_LINE(3)  LOAD_WHEEL_LINE(4)
		LOAD_WHEEL_LINE(5)  LOAD_WHEEL_LINE(6)  LOAD_WHEEL_LINE(7)  LOAD_WHEEL_LINE(8)
		LOAD_WHEEL_LINE(9)  LOAD_WHEEL_LINE(10) LOAD_WHEEL_LINE(11) LOAD_WHEEL_LINE(12)
		LOAD_WHEEL_LINE(13) LOAD_WHEEL_LINE(14) LOAD_WHEEL_LINE(15) LOAD_WHEEL_LINE(16)
		LOAD_WHEEL_LINE(17) LOAD_WHEEL_LINE(18) LOAD_WHEEL_LINE(19) LOAD_WHEEL_LINE(20)
		LOAD_WHEEL_LINE(21) LOAD_WHEEL_LINE(22) LOAD_WHEEL_LINE(23) LOAD_WHEEL_LINE(24)
		LOAD_WHEEL_LINE(25) LOAD_WHEEL_LINE(26) LOAD_WHEEL_LINE(27) LOAD_WHEEL_LINE(28)
		LOAD_WHEEL_LINE(29) LOAD_WHEEL_LINE(30) LOAD_WHEEL_LINE(31) LOAD_WHEEL_LINE(32)
#undef LOAD_WHEEL_LINE

		// Innerwheelring1
		if (objLoader.LoadModel("TestModels/Innerwheelring1.obj")) { innerwheelring1.ConstructModelFromOBJLoader(objLoader); innerwheelring1.InitVBO(myShader); }
		else cout << " model failed to load Innerwheelring1" << endl;
	// Innerwheelring2
	if (objLoader.LoadModel("TestModels/Innerwheelring2.obj")) { innerwheelring2.ConstructModelFromOBJLoader(objLoader); innerwheelring2.InitVBO(myShader); }
	else cout << " model failed to load Innerwheelring2" << endl;

	// Innerrect 1-16
#define LOAD_INNER_RECT(n) \
        if (objLoader.LoadModel("TestModels/Innerrect" #n ".obj")) { innerrect##n.ConstructModelFromOBJLoader(objLoader); innerrect##n.InitVBO(myShader); } \
        else cout << " model failed to load Innerrect" #n << endl;
	LOAD_INNER_RECT(1)  LOAD_INNER_RECT(2)  LOAD_INNER_RECT(3)  LOAD_INNER_RECT(4)
		LOAD_INNER_RECT(5)  LOAD_INNER_RECT(6)  LOAD_INNER_RECT(7)  LOAD_INNER_RECT(8)
		LOAD_INNER_RECT(9)  LOAD_INNER_RECT(10) LOAD_INNER_RECT(11) LOAD_INNER_RECT(12)
		LOAD_INNER_RECT(13) LOAD_INNER_RECT(14) LOAD_INNER_RECT(15) LOAD_INNER_RECT(16)
#undef LOAD_INNER_RECT

		// Stands 1-4
	if (objLoader.LoadModel("TestModels/Stand1.obj")) { stand1.ConstructModelFromOBJLoader(objLoader); stand1.InitVBO(myShader); }
	else cout << " model failed to load Stand1" << endl;
	if (objLoader.LoadModel("TestModels/Stand2.obj")) { stand2.ConstructModelFromOBJLoader(objLoader); stand2.InitVBO(myShader); }
	else cout << " model failed to load Stand2" << endl;
	if (objLoader.LoadModel("TestModels/Stand3.obj")) { stand3.ConstructModelFromOBJLoader(objLoader); stand3.InitVBO(myShader); }
	else cout << " model failed to load Stand3" << endl;
	if (objLoader.LoadModel("TestModels/Stand4.obj")) { stand4.ConstructModelFromOBJLoader(objLoader); stand4.InitVBO(myShader); }
	else cout << " model failed to load Stand4" << endl;

	// load the blinking-bulb model
	for (int i = 1; i <= 32; ++i) {
		std::string path = "TestModels/light" + std::to_string(i) + ".obj";
		if (objLoader.LoadModel(path.c_str())) {
			lights.emplace_back();                                  // add a new CThreeDModel
			lights.back().ConstructModelFromOBJLoader(objLoader);
			lights.back().InitVBO(myShader);
		}
		else {
			std::cerr << "model failed to load " << path << "\n";
		}
	}


	// load cart1
	if (objLoader.LoadModel("TestModels/cart1.obj")) {
		cart1.ConstructModelFromOBJLoader(objLoader);
		cart1.InitVBO(myShader);

		carts.push_back(&cart1);
		cartOffsets.push_back(glm::vec3(0.0f, 776.0f, 0.0f));
	}

	if (objLoader.LoadModel("TestModels/cart1Top.obj")) {
		cart1Top.ConstructModelFromOBJLoader(objLoader);
		cart1Top.InitVBO(myShader);
		carts.push_back(&cart1Top);
		cartOffsets.push_back(glm::vec3(0.0f, 776.0f, 0.0f));  // same pivot as cart1
	}

	// load cart2
	if (objLoader.LoadModel("TestModels/cart2.obj")) {
		cart2.ConstructModelFromOBJLoader(objLoader);
		cart2.InitVBO(myShader);

		carts.push_back(&cart2);
		cartOffsets.push_back(glm::vec3(-295.0f, 716.0f, 00.0f));
	}

	if (objLoader.LoadModel("TestModels/cart3.obj")) {
		cart3.ConstructModelFromOBJLoader(objLoader);
		cart3.InitVBO(myShader);

		carts.push_back(&cart3);
		cartOffsets.push_back(glm::vec3(-547.0f, 550.0f, 00.0f));
	}

	if (objLoader.LoadModel("TestModels/cart4.obj")) {
		cart4.ConstructModelFromOBJLoader(objLoader);
		cart4.InitVBO(myShader);

		carts.push_back(&cart4);
		cartOffsets.push_back(glm::vec3(-716.0f, 297.0f, 0.0f));
	}

	if (objLoader.LoadModel("TestModels/cart5.obj")) {
		cart5.ConstructModelFromOBJLoader(objLoader);
		cart5.InitVBO(myShader);

		carts.push_back(&cart5);
		cartOffsets.push_back(glm::vec3(-776.0f, 1.0f, 0.0f));
	}

	if (objLoader.LoadModel("TestModels/cart6.obj")) {
		cart6.ConstructModelFromOBJLoader(objLoader);
		cart6.InitVBO(myShader);

		carts.push_back(&cart6);
		cartOffsets.push_back(glm::vec3(-715.0f, -295.0f, 0.0f));
	}

	if (objLoader.LoadModel("TestModels/cart7.obj")) {
		cart7.ConstructModelFromOBJLoader(objLoader);
		cart7.InitVBO(myShader);

		carts.push_back(&cart7);
		cartOffsets.push_back(glm::vec3(-547.0f, -548.0f, 0.0f));
	}

	if (objLoader.LoadModel("TestModels/cart8.obj")) {
		cart8.ConstructModelFromOBJLoader(objLoader);
		cart8.InitVBO(myShader);

		carts.push_back(&cart8);
		cartOffsets.push_back(glm::vec3(-300.0f, -715.0f, 0.0f));
	}

	if (objLoader.LoadModel("TestModels/cart9.obj")) {
		cart9.ConstructModelFromOBJLoader(objLoader);
		cart9.InitVBO(myShader);

		carts.push_back(&cart9);
		cartOffsets.push_back(glm::vec3(0.0f, -776.0f, 0.0f));
	}

	if (objLoader.LoadModel("TestModels/cart10.obj")) {
		cart10.ConstructModelFromOBJLoader(objLoader);
		cart10.InitVBO(myShader);

		carts.push_back(&cart10);
		cartOffsets.push_back(glm::vec3(300.0f, -715.0f, 0.0f));
	}

	if (objLoader.LoadModel("TestModels/cart11.obj")) {
		cart11.ConstructModelFromOBJLoader(objLoader);
		cart11.InitVBO(myShader);

		carts.push_back(&cart11);
		cartOffsets.push_back(glm::vec3(547.0f, -548.0f, 0.0f));
	}

	if (objLoader.LoadModel("TestModels/cart12.obj")) {
		cart12.ConstructModelFromOBJLoader(objLoader);
		cart12.InitVBO(myShader);

		carts.push_back(&cart12);
		cartOffsets.push_back(glm::vec3(715.0f, -295.0f, 0.0f));
	}

	if (objLoader.LoadModel("TestModels/cart13.obj")) {
		cart13.ConstructModelFromOBJLoader(objLoader);
		cart13.InitVBO(myShader);

		carts.push_back(&cart13);
		cartOffsets.push_back(glm::vec3(776.0f, 1.0f, 0.0f));
	}

	if (objLoader.LoadModel("TestModels/cart14.obj")) {
		cart14.ConstructModelFromOBJLoader(objLoader);
		cart14.InitVBO(myShader);

		carts.push_back(&cart14);
		cartOffsets.push_back(glm::vec3(716.0f, 297.0f, 0.0f));
	}

	if (objLoader.LoadModel("TestModels/cart15.obj")) {
		cart15.ConstructModelFromOBJLoader(objLoader);
		cart15.InitVBO(myShader);

		carts.push_back(&cart15);
		cartOffsets.push_back(glm::vec3(547.0f, 550.0f, 0.0f));
	}
	if (objLoader.LoadModel("TestModels/cart16.obj")) {
		cart16.ConstructModelFromOBJLoader(objLoader);
		cart16.InitVBO(myShader);

		carts.push_back(&cart16);
		cartOffsets.push_back(glm::vec3(295.0f, 716.0f, 0.0f));
	}


	glutSetCursor(GLUT_CURSOR_NONE);
}

void keyboard(unsigned char key, int x, int y)
{
	keyState[key] = true;

	if (key == '1') currentCameraMode = FREE_CAMERA;
	else if (key == '2') currentCameraMode = CART_CAMERA;
	else if (key == '3') currentCameraMode = FIXED_CAMERA;
	else if (key == '4') currentCameraMode = FIXED_CAMERA_2;
	else if (key == '5') currentCameraMode = FIXED_CAMERA_3;


	if (key == 'r') {
		// this will cycle 1,2,3,1
		currentLightMode = LightMode((currentLightMode + 1) % 3);
	}

}

void keyboardUp(unsigned char key, int x, int y)
{
	keyState[key] = false; // Set the key state to false when released
}

void special(int key, int x, int y)
{
}

void specialUp(int key, int x, int y)
{
}

// returns true if worldPos is inside *any* of your meshes
bool CheckCollision(const glm::vec3& worldPos) {

	// 1) prepare the inverted rotation for all the wheel’s spinning parts
	glm::mat4 rotM = glm::rotate(glm::mat4(1.0f),
		glm::radians(wheelRotationAngle),
		glm::vec3(0, 0, 1));
	glm::mat4 invRotM = glm::inverse(rotM);

	// 2) prepare the inverted translation for the cart top (if you ever need it)
	//    (we'll recompute per?cart below)

	// --- A) static geometry (identity transform) ---
	for (auto m : { &theFloor, &bottompart, &centerblock,
					&stand1, &stand2, &stand3, &stand4 })
	{
		if (m->IsPointInLeaf(worldPos.x, worldPos.y, worldPos.z))
			return true;
	}

	// --- B) rotating wheel pieces ---
	for (auto m : {
		&wheelringfront1,& wheelringfront2,
		& wheelline1,& wheelline2,& wheelline3,& wheelline4,
		& wheelline5,& wheelline6,& wheelline7,& wheelline8,
		& wheelline9,& wheelline10,& wheelline11,& wheelline12,
		& wheelline13,& wheelline14,& wheelline15,& wheelline16,
		& wheelline17,& wheelline18,& wheelline19,& wheelline20,
		& wheelline21,& wheelline22,& wheelline23,& wheelline24,
		& wheelline25,& wheelline26,& wheelline27,& wheelline28,
		& wheelline29,& wheelline30,& wheelline31,& wheelline32,
		& innerwheelring1,& innerwheelring2,
		& innerrect1,& innerrect2,& innerrect3,& innerrect4,
		& innerrect5,& innerrect6,& innerrect7,& innerrect8,
		& innerrect9,& innerrect10,& innerrect11,& innerrect12,
		& innerrect13,& innerrect14,& innerrect15,& innerrect16,
		& centerstar
		})
	{
		glm::vec3 local = glm::vec3(invRotM * glm::vec4(worldPos, 1.0f));
		if (m->IsPointInLeaf(local.x, local.y, local.z))
			return true;
	}

	// C) lights (blinking spheres) – treat them like any other mesh
	for (auto& L : lights) {
		// if your lights only rotate, you may need the same invRotM transformation:
		glm::vec3 local = glm::vec3(invRotM * glm::vec4(worldPos, 1.0f));
		if (L.IsPointInLeaf(local.x, local.y, local.z))
			return true;
	}

	// --- D) moving carts (orbiting) ---
	// For each cart: compute the same rotation-pivot you use in display(),
	// then invert that translation to bring worldPos into cart-model space.
	for (size_t i = 0; i < carts.size(); ++i) {
		CThreeDModel* cart = carts[i];

		const glm::vec3& offset = cartOffsets[i];

		// where the pivot sent this cart this frame:
		glm::vec3 rotatedPos = glm::vec3(rotM * glm::vec4(offset, 1.0f));
		glm::vec3 deltaPos = rotatedPos - offset;

		// build and invert the cart's model?space translation:
		glm::mat4 cartM = glm::translate(glm::mat4(1.0f), deltaPos);
		glm::mat4 invCartM = glm::inverse(cartM);

		// transform the query point into cart?local coords:
		glm::vec3 local = glm::vec3(invCartM * glm::vec4(worldPos, 1.0f));

		if (cart->IsPointInLeaf(local.x, local.y, local.z))
			return true;
	}

	return false;
}


void processKeys() {
	// Always allow ESC to exit
	if (keyState[27]) exit(0);

	// Always allow wheel rotation
	if (keyState['q']) wheelRotationAngle += wheelRotationSpeed;
	if (keyState['e']) wheelRotationAngle -= wheelRotationSpeed;
	if (wheelRotationAngle >= 360.0f) wheelRotationAngle -= 360.0f;
	if (wheelRotationAngle < 0.0f) wheelRotationAngle += 360.0f;

	// Block movement keys if not in free camera mode
	if (currentCameraMode != FREE_CAMERA)
		return;

	// Movement logic (WASD, space, Z)
	glm::vec3 forward = glm::normalize(cameraTarget - cameraPos);
	glm::vec3 right = glm::normalize(glm::cross(forward, cameraUp));
	glm::vec3 up = cameraUp;

	auto tryMove = [&](const glm::vec3& delta) {
		glm::vec3 np = cameraPos + delta;
		if (!CheckCollision(np)) {
			cameraPos = np;
			cameraTarget += delta;
		}
		};

	if (keyState['w']) tryMove(forward * cameraSpeed);
	if (keyState['s']) tryMove(-forward * cameraSpeed);
	if (keyState['a']) tryMove(-right * cameraSpeed);
	if (keyState['d']) tryMove(right * cameraSpeed);
	if (keyState[32])  tryMove(up * cameraSpeed);    // space
	if (keyState['z']) tryMove(-up * cameraSpeed);   // fly down
}



void mouse_callback(int xpos, int ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraTarget = glm::normalize(direction) + cameraPos;

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
/**************** END OPENGL FUNCTIONS *************************/

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
