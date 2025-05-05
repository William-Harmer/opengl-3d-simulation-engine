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

// Existing models
CThreeDModel wheelBase, wheel, cart, theFloor;
// Individual new models
CThreeDModel centerstar, bottompart, centerblock;
CThreeDModel wheelringfront1, wheelringfront2;
CThreeDModel wheelline1, wheelline2, wheelline3, wheelline4, wheelline5, wheelline6, wheelline7, wheelline8;
CThreeDModel wheelline9, wheelline10, wheelline11, wheelline12, wheelline13, wheelline14, wheelline15, wheelline16;
CThreeDModel wheelline17, wheelline18, wheelline19, wheelline20, wheelline21, wheelline22, wheelline23, wheelline24;
CThreeDModel wheelline25, wheelline26, wheelline27, wheelline28, wheelline29, wheelline30, wheelline31, wheelline32;
CThreeDModel innerwheelring1, innerwheelring2;
CThreeDModel innerrect1, innerrect2, innerrect3, innerrect4, innerrect5, innerrect6, innerrect7, innerrect8;
CThreeDModel innerrect9, innerrect10, innerrect11, innerrect12, innerrect13, innerrect14, innerrect15, innerrect16;
CThreeDModel stand1, stand2, stand3, stand4;

COBJLoader objLoader;

float amount = 0;
float temp = 0.002f;

///END MODEL LOADING

glm::mat4 ProjectionMatrix; // matrix for the orthographic projection
glm::mat4 ModelViewMatrix;  // matrix for the modelling and viewing

//glm::mat4 objectRotation;
//glm::vec3 translation = glm::vec3(0.0, 0.0, 0.0);
//glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f); //vector for the position of the object.

//Material properties
float Material_Ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
float Material_Diffuse[4] = { 0.8f, 0.8f, 0.5f, 1.0f };
float Material_Specular[4] = { 0.9f,0.9f,0.8f,1.0f };
float Material_Shininess = 50;

//Light Properties
float Light_Ambient_And_Diffuse[4] = { 0.8f, 0.8f, 0.6f, 1.0f };
float Light_Specular[4] = { 1.0f,1.0f,1.0f,1.0f };
float LightPos[4] = { 0.0f, 0.0f, 1.0f, 0.0f };

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



/*************    START OF OPENGL FUNCTIONS   ****************/
void display()
{
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
	glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
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

	// --- Draw wheel base ---
	{
		glm::mat4 mv = view * glm::mat4(1.0f);
		glm::mat3 nm = glm::inverseTranspose(glm::mat3(mv));
		glUniformMatrix3fv(glGetUniformLocation(prog, "NormalMatrix"), 1, GL_FALSE, glm::value_ptr(nm));
		glUniformMatrix4fv(glGetUniformLocation(prog, "ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(mv));
		wheelBase.DrawElementsUsingVBO(myShader);
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

	// --- Finally, draw the cart with its translation only ---
	{
		glm::mat4 modelCart = glm::translate(glm::mat4(1.0f), cartTopPos);
		glm::mat4 mv = view * modelCart;
		glm::mat3 nm = glm::inverseTranspose(glm::mat3(mv));
		glUniformMatrix3fv(glGetUniformLocation(prog, "NormalMatrix"), 1, GL_FALSE, glm::value_ptr(nm));
		glUniformMatrix4fv(glGetUniformLocation(prog, "ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(mv));
		cart.DrawElementsUsingVBO(myShader);
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
	ProjectionMatrix = glm::perspective(glm::radians(60.0f), (GLfloat)screenWidth / (GLfloat)screenHeight, 1.0f, 4000.0f);
}

void init()
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
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
	// Wheel base
	if (objLoader.LoadModel("TestModels/wheelbase.obj")) { wheelBase.ConstructModelFromOBJLoader(objLoader); wheelBase.InitVBO(myShader); }
	else cout << " model failed to load wheelbase" << endl;
	// Wheel
	if (objLoader.LoadModel("TestModels/wheel.obj")) { wheel.ConstructModelFromOBJLoader(objLoader); wheel.InitVBO(myShader); }
	else cout << " model failed to load wheel" << endl;
	// Cart
	if (objLoader.LoadModel("TestModels/cart.obj")) { cart.ConstructModelFromOBJLoader(objLoader); cart.InitVBO(myShader); }
	else cout << " model failed to load cart" << endl;

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

	glutSetCursor(GLUT_CURSOR_NONE);
}

void keyboard(unsigned char key, int x, int y)
{
	keyState[key] = true;  // Set the key state to true when pressed
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

void processKeys()
{
	glm::vec3 forward = glm::normalize(cameraTarget - cameraPos);  // forward vector
	glm::vec3 right = glm::normalize(glm::cross(forward, cameraUp));  // right vector
	glm::vec3 up = cameraUp;  // camera's up vector (for flying)

	// WASD movement
	if (keyState['w']) {
		cameraPos += forward * cameraSpeed;
		cameraTarget += forward * cameraSpeed;
	}
	if (keyState['s']) {
		cameraPos -= forward * cameraSpeed;
		cameraTarget -= forward * cameraSpeed;
	}
	if (keyState['a']) {
		cameraPos -= right * cameraSpeed;
		cameraTarget -= right * cameraSpeed;
	}
	if (keyState['d']) {
		cameraPos += right * cameraSpeed;
		cameraTarget += right * cameraSpeed;
	}

	if (keyState[32]) {  // 32 is ASCII for space
		cameraPos += up * cameraSpeed;
		cameraTarget += up * cameraSpeed;
	}

	// Flying down (Shift key)
	if (keyState['z']) {
		cameraPos -= up * cameraSpeed;
		cameraTarget -= up * cameraSpeed;
	}

	if (keyState[27]) // Escape
	{
		exit(0);
	}

	if (keyState['q']) {
		wheelRotationAngle += wheelRotationSpeed;
	}
	if (keyState['e']) {
		wheelRotationAngle -= wheelRotationSpeed;
	}

	if (wheelRotationAngle >= 360.0f) {
		wheelRotationAngle -= 360.0f;
	}
	if (wheelRotationAngle < 0.0f) {
		wheelRotationAngle += 360.0f;
	}

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