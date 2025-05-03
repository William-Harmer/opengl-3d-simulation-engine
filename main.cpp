#include <iostream>
using namespace std;


//--- OpenGL ---
#include "GL\glew.h"
#include "GL\wglew.h"
#pragma comment(lib, "glew32.lib")
//--------------

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_inverse.hpp"

#include "GL\freeglut.h"

#include "Images\FreeImage.h"

#include "shaders\Shader.h"

CShader* myShader;  ///shader object 
//CShader* myBasicShader;

//MODEL LOADING
#include "3DStruct\threeDModel.h"
#include "Obj\OBJLoader.h"

float amount = 0;
float temp = 0.002f;

CThreeDModel wheelBase, wheel, cart, theFloor; //A threeDModel object is needed for each model loaded
COBJLoader objLoader;	//this object is used to load the 3d models.
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
	//cout << "Camera Position: (" << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << ")" << endl;
	//cout << "Model Position: (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << endl;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(myShader->GetProgramObjID());  // use the shader

	//Part for displacement shader.
	amount += temp;
	if (amount > 1.0f || amount < -1.5f)
		temp = -temp;
	//amount = 0;
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "displacement"), amount);

	//Set the projection matrix in the shader
	GLuint projMatLocation = glGetUniformLocation(myShader->GetProgramObjID(), "ProjectionMatrix");
	glUniformMatrix4fv(projMatLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);

	glm::mat4 viewingMatrix = glm::lookAt(cameraPos, cameraTarget, cameraUp);

	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ViewMatrix"), 1, GL_FALSE, &viewingMatrix[0][0]);

	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "LightPos"), 1, LightPos);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_ambient"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_diffuse"), 1, Light_Ambient_And_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "light_specular"), 1, Light_Specular);

	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_ambient"), 1, Material_Ambient);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_diffuse"), 1, Material_Diffuse);
	glUniform4fv(glGetUniformLocation(myShader->GetProgramObjID(), "material_specular"), 1, Material_Specular);
	glUniform1f(glGetUniformLocation(myShader->GetProgramObjID(), "material_shininess"), Material_Shininess);

	//pos.x += objectRotation[2][0]*0.0003;
	//pos.y += objectRotation[2][1]*0.0003;
	//pos.z += objectRotation[2][2]*0.0003;

	//glm::mat4 modelmatrix = glm::translate(glm::mat4(1.0f), pos);
	//ModelViewMatrix = viewingMatrix * modelmatrix * objectRotation;
	//glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	//model.DrawElementsUsingVBO(myShader);

	//Switch to basic shader to draw the lines for the bounding boxes
	//glUseProgram(myBasicShader->GetProgramObjID());
	//projMatLocation = glGetUniformLocation(myBasicShader->GetProgramObjID(), "ProjectionMatrix");
	//glUniformMatrix4fv(projMatLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);
	//glUniformMatrix4fv(glGetUniformLocation(myBasicShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	//model.DrawAllBoxesForOctreeNodes(myBasicShader);
	//model.DrawBoundingBox(myBasicShader);
	//	model.CalcBoundingBox()
	//model.DrawOctreeLeaves(myBasicShader);


	//glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	//glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	//switch back to the shader for textures and lighting on the objects.
	glUseProgram(myShader->GetProgramObjID());  // use the shader

	// Ok so calculating the centre point is only in its local space
	// But we know if we do 0,0,0 that the objects are being centered.
	// But if the objects are centeres why is the cart not going to the middle?
	//Vector3d* centre = wheel.GetCentrePoint();
	//glm::vec3 wheelCentre(centre->x, centre->y, centre->z);
	//std::cout << "Centre Point: (" << centre->x << ", " << centre->y << ", " << centre->z << ")" << std::endl;
	//std::cout << "Wheel Centre: (" << wheelCentre.x << ", " << wheelCentre.y << ", " << wheelCentre.z << ")" << std::endl;

	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(0, 0, 0));
	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	theFloor.DrawElementsUsingVBO(myShader);


	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(0, 0, 0));
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
	wheelBase.DrawElementsUsingVBO(myShader);
	//wheelBase.DrawBoundingBox(myShader);


	// Apply the rotation transformation to the wheel
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(wheelRotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = glm::translate(viewingMatrix, glm::vec3(0, 0, 0)) * rotationMatrix;
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	wheel.DrawElementsUsingVBO(myShader);

	// Step 1: Build the Model Matrix (Local ? World)
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(wheelRotationAngle), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around origin
	modelMatrix = glm::translate(modelMatrix, cartTopPos); // Move cart to its position in world space

	// Step 2: Build the Model-View Matrix (Local ? Eye)
	glm::mat4 ModelViewMatrix = viewingMatrix * modelMatrix;

	// Step 3: Compute and upload Normal Matrix
	normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix3fv(glGetUniformLocation(myShader->GetProgramObjID(), "NormalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);

	// Step 4: Upload Model-View Matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(myShader->GetProgramObjID(), "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);

	// Step 5: Define the local position of the point you want to track
	glm::vec4 localPoint = glm::vec4(0.0f, 10.0f, 0.0f, 1.0f);

	// Step 6: Transform to world space (Model Matrix only)
	glm::vec3 worldPoint = glm::vec3(modelMatrix * localPoint);

	// Step 8: Print the world position
	std::cout << "World Position of Cart Point: (" << worldPoint.x << ", " << worldPoint.y << ", " << worldPoint.z << ")" << std::endl;

	// Step 9: Draw the object
	cart.DrawElementsUsingVBO(myShader);

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
	glClearColor(1.0, 1.0, 1.0, 0.0);						//sets the clear colour to yellow
	//glClear(GL_COLOR_BUFFER_BIT) in the display function
	//will clear the buffer to this colour
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);


	myShader = new CShader();
	//if(!myShader->CreateShaderProgram("BasicView", "glslfiles/basicTransformationsWithDisplacement.vert", "glslfiles/basicTransformationsWithDisplacement.frag"))
	if (!myShader->CreateShaderProgram("BasicView", "glslfiles/basicTransformations.vert", "glslfiles/basicTransformations.frag"))
	{
		cout << "failed to load shader" << endl;
	}

	//myBasicShader = new CShader();
	//if (!myBasicShader->CreateShaderProgram("Basic", "glslfiles/basic.vert", "glslfiles/basic.frag"))
	//{
	//	cout << "failed to load shader" << endl;
	//}

	glUseProgram(myShader->GetProgramObjID());  // use the shader

	glEnable(GL_TEXTURE_2D);

	//lets initialise our object's rotation transformation 
	//to the identity matrix
	//objectRotation = glm::mat4(1.0f);


	cout << " loading model " << endl;
	if (objLoader.LoadModel("TestModels/floor.obj"))//returns true if the model is loaded
	{
		cout << " model loaded " << endl;

		//copy data from the OBJLoader object to the threedmodel class
		theFloor.ConstructModelFromOBJLoader(objLoader);



		//if you want to translate the object to the origin of the screen,
		//first calculate the centre of the object, then move all the vertices
		//back so that the centre is on the origin.
		/*model.CalcCentrePoint();
		model.CentreOnZero();*/


		theFloor.InitVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}

	if (objLoader.LoadModel("TestModels/wheelbase.obj"))//returns true if the model is loaded
	{
		wheelBase.ConstructModelFromOBJLoader(objLoader);
		wheelBase.InitVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}
	if (objLoader.LoadModel("TestModels/wheel.obj"))//returns true if the model is loaded
	{
		wheel.ConstructModelFromOBJLoader(objLoader);
		wheel.InitVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}
	if (objLoader.LoadModel("TestModels/cart.obj"))//returns true if the model is loaded
	{
		cart.ConstructModelFromOBJLoader(objLoader);
		cart.InitVBO(myShader);
	}
	else
	{
		cout << " model failed to load " << endl;
	}
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