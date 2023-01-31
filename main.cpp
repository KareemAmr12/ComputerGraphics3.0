#include <GL/glut.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <string>
using namespace std;
#define PI 3.141592

#include <iostream>
#define std

#define BODY_WIDTH 2
#define BODY_HEIGHT 4
#define BODY_DEPTH 2

void mymenu(int value);
void robotColorMenu(int value);
void dogColorMenu(int value);
void catColorMenu(int value);
void creeperColorMenu(int value);
void drawMenu(int value);

// Camera Varaibles
static size_t windowWidth = 640;
static size_t windowHeight = 480;
static float aspectRatio;
GLint leftMouseButton, rightMouseButton; //status of the mouse buttons
int mouseX = 0, mouseY = 0; //last known X and Y of the mouse
float cameraTheta, cameraPhi, cameraRadius; //camera position in spherical coordinates
float x, y, z; //camera position in cartesian coordinates

float shoulderZ = 0.0;
float shoulderY = 1.0;
bool ToggAxis = true;

// Robot Joints
static float shoulderAngle = 0, elbowAngle = -10, hipAngle = 0, kneeAngle = 10;
float moveX = 0, moveY = 0, moveZ = 0;
float spinAngle = 0;
bool displayWireMesh;

bool EasterEgg = false;
bool displayRobot = true;

// Robot Colors
float redHeadRobot = 0.0f;
float greenHeadRobot = 1.0f;
float blueHeadRobot = 0.0f;

float redBodyRobot = 1.0f;
float greenBodyRobot = 0.0f;
float blueBodyRobot = 0.0f;

float RedRobotPartColor = 0.0f;
float GreenRobotPartColor = 0.0f;
float BlueRobotPartColor = 1.0f;

// Animation Variables
bool playAnimation = true;
bool visiblePath = true;
bool linePath = true;
bool circlePath = false;
bool resetOn = false;

int aniSeconds = 0;
int aniState = 0;

void* m_glutFont = GLUT_BITMAP_TIMES_ROMAN_24;

void recomputeOrientation()
{
	x = cameraRadius * sinf(cameraTheta) * sinf(cameraPhi);
	z = cameraRadius * cosf(cameraTheta) * sinf(cameraPhi);
	y = cameraRadius * cosf(cameraPhi);
	glutPostRedisplay();
}


void drawText(const std::string& text, const unsigned int x, const unsigned int y, const float r, const float g, const float b)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, windowWidth, 0, windowHeight, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(r, g, b);
	glRasterPos2i(x, y);
	for (const char c : text)
		glutBitmapCharacter(m_glutFont, (int)c);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void drawBox(GLdouble width, GLdouble height, GLdouble depth, float red, float green, float blue) {
	if (displayWireMesh)
	{
		glPushMatrix();
		glColor3f(1, 1, 1);
		glScalef(width, height, depth);
		glutWireCube(1.0);
		glPopMatrix();
	}
	else
	{
		glPushMatrix();
		glColor3f(red, green, blue);
		glScalef(width, height, depth);
		glutSolidCube(1.0);
		glPopMatrix();
	}
}

void drawSphere(GLdouble width, GLdouble height, GLdouble depth, float red, float green, float blue) {
	if (displayWireMesh)
	{
		glPushMatrix();
		glColor3f(1, 1, 1);
		glScalef(width, height, depth);
		glutWireSphere(1.0f, 10, 10);
		glPopMatrix();
	}
	else
	{
		glPushMatrix();
		glColor3f(red, green, blue);
		glScalef(width, height, depth);
		glutSolidSphere(1.0f, 10, 10);
		glPopMatrix();
	}
}

void mouseCallback(int button, int state, int thisX, int thisY)
{
	// Update the left and right mouse button states, if applicable
	if (button == GLUT_LEFT_BUTTON)
		leftMouseButton = state;
	else if (button == GLUT_RIGHT_BUTTON)
		rightMouseButton = state;
	// and update the last seen X and Y coordinates of the mouse
	mouseX = thisX;
	mouseY = thisY;
}

void mouseMotion(int x, int y)
{
	if (leftMouseButton == GLUT_DOWN)
	{
		cameraTheta += (mouseX - x) * 0.005;
		cameraPhi += (mouseY - y) * 0.005;
		// Make sure that phi stays within the range (0, PI)
		if (cameraPhi <= 0)
			cameraPhi = 0 + 0.001;
		if (cameraPhi >= PI)
			cameraPhi = PI - 0.001;
		recomputeOrientation(); // Update camera (x, y, z)
	}
	// Camera zoom in/out
	else if (rightMouseButton == GLUT_DOWN)
	{
		double totalChangeSq = (x - mouseX) + (y - mouseY);

		cameraRadius += totalChangeSq * 0.01;

		// Limit the camera radius to some reasonable values so the user can't get lost
		if (cameraRadius < 2.0)
			cameraRadius = 2.0;
		if (cameraRadius > 10.0)
			cameraRadius = 10.0;
		recomputeOrientation(); // Update camera (x, y, z) based on (radius, theta, phi)
	}
	mouseX = x;
	mouseY = y;
}

void drawLine()
{
	glBegin(GL_LINES);
	
	glColor3f(1.0f,1.0f,1.0f);
	glVertex2f(0, 0);
	glVertex2f(-10, -10);
	glEnd();
}

// Draw Circle
void drawCircle(GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLint numberOfSides)
{
	GLint numberOfVertices = numberOfSides + 1;

	GLfloat doublePi = 2.0f * PI;

	GLfloat circleVerticesX[20];
	GLfloat circleVerticesY[20];
	GLfloat circleVerticesZ[20];

	for (int i = 0; i < numberOfVertices; i++)
	{
		circleVerticesX[i] = x + (radius * cos(i * doublePi / numberOfSides));
		circleVerticesY[i] = y + (radius * sin(i * doublePi / numberOfSides));
		circleVerticesZ[i] = z;
	}

	GLfloat allCircleVertices[20 * 3];

	for (int i = 0; i < numberOfVertices; i++)
	{
		allCircleVertices[i * 3] = circleVerticesX[i];
		allCircleVertices[(i * 3) + 1] = circleVerticesY[i];
		allCircleVertices[(i * 3) + 2] = circleVerticesZ[i];
	}
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, allCircleVertices);
	glDrawArrays(GL_LINE_STRIP, 0, numberOfVertices);
	glDisableClientState(GL_VERTEX_ARRAY);
}



void drawRobot()
{
	
	//
	// From my perspective
	// Body
	glPushMatrix();
	glTranslatef(0, 0, 0);
	drawBox(BODY_WIDTH, BODY_HEIGHT, BODY_DEPTH, redBodyRobot, greenBodyRobot, blueBodyRobot);
	glPopMatrix();

	// Head
	glPushMatrix();
	glTranslatef(0, 2.9f, 0);
	drawSphere(1.0, 1.0, 1.0, redHeadRobot, greenHeadRobot, blueHeadRobot);
	glPopMatrix();

	// Right Hand
	glPushMatrix();
	glTranslatef(1.0f, 1.5f, 0);
	glRotatef((GLfloat)-83, 0.0, 0.0, 1.0);
	glRotatef((GLfloat)-shoulderAngle, 0.0, shoulderY, shoulderZ);
	glTranslatef(1.0f, 0, 0);
	drawBox(2.0, 0.4, 1.0, RedRobotPartColor, GreenRobotPartColor, BlueRobotPartColor); // Upper arm

	glTranslatef(1.0f, 0, 0);
	glRotatef((GLfloat)-elbowAngle, 0.0, 1.0, 0.0);
	glTranslatef(1.0f, 0, 0);
	drawBox(2.0, 0.4, 1.0, RedRobotPartColor, GreenRobotPartColor, BlueRobotPartColor); // Lower arm
	glPopMatrix();

	// left Hand
	glPushMatrix();
	glTranslatef(-1.0f, 1.5f, 0);
	glRotatef((GLfloat)-97, 0.0, 0.0, 1.0);
	glRotatef((GLfloat)shoulderAngle, 0.0, shoulderY, shoulderZ);
	glTranslatef(1.0f, 0, 0);
	drawBox(2.0, 0.4, 1.0, RedRobotPartColor, GreenRobotPartColor, BlueRobotPartColor); // Upper arm

	glTranslatef(1.0f, 0, 0);
	glRotatef((GLfloat)-elbowAngle, 0.0, 1.0, 0.0);
	glTranslatef(1.0f, 0, 0);
	drawBox(2.0, 0.4, 1.0, RedRobotPartColor, GreenRobotPartColor, BlueRobotPartColor); // Lower arm
	glPopMatrix();


	// right leg
	glPushMatrix();
	glTranslatef(0.7f, -2.0f, 0);
	glRotatef((GLfloat)-90, 0.0, 0.0, 1.0);
	glRotatef((GLfloat)hipAngle, 0.0, 1.0, 0.0);
	glTranslatef(1.0f, 0, 0);
	drawBox(2.0, 0.6, 1.0, RedRobotPartColor, GreenRobotPartColor, BlueRobotPartColor); // 

	glTranslatef(1.0f, 0, 0);
	glRotatef((GLfloat)-kneeAngle, 0.0, 1.0, 0.0);
	glTranslatef(1.0f, 0, 0);
	drawBox(2.0, 0.6, 1.0, RedRobotPartColor, GreenRobotPartColor, BlueRobotPartColor); // Lower arm
	glPopMatrix();


	// right leg
	glPushMatrix();
	glTranslatef(-0.7f, -2.0f, 0);
	glRotatef((GLfloat)-90, 0.0, 0.0, 1.0);
	glRotatef((GLfloat)-hipAngle, 0.0, 1.0, 0.0);
	glTranslatef(1.0f, 0, 0);
	drawBox(2.0, 0.6, 1.0, RedRobotPartColor, GreenRobotPartColor, BlueRobotPartColor); // 

	glTranslatef(1.0f, 0, 0);
	glRotatef((GLfloat)-kneeAngle, 0.0, 1.0, 0.0);
	glTranslatef(1.0f, 0, 0);
	drawBox(2.0, 0.6, 1.0, RedRobotPartColor, GreenRobotPartColor, BlueRobotPartColor); // Lower arm
	glPopMatrix();
	
}
void axis()
{
	glPushMatrix();
	// Draw a red x-axis, a green y-axis and a blue z-axis
	
	glBegin(GL_LINES);

	glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(10, 0, 0);
	glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, 10, 0);
	glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 10);
	glEnd();
	glPopMatrix();
}

void drawScene()
{
	glPushMatrix();
	glTranslatef(0.0f, -5.0f, 0.0f);
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glVertex3f(-100.0f, 0.0f, -100.0f);
	glVertex3f(-100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, -100.0f);
	glEnd();
	glPopMatrix();
	glPushMatrix();

	//for the easter egg we just draw the text and we use gltranslate but in this case to fly the robot away from the scene.
	if (EasterEgg)
	{
		std::string name;
		name = "*insert angelic sound*";
		drawText(name.data(), windowWidth / 2 - 120, windowHeight / 2 + 20, 1.0,1.0,1.0);
	}
	if (linePath || EasterEgg)
	{
		glTranslatef(moveX, moveY, moveZ);
	}
	if (circlePath)
	{
		glRotatef(spinAngle, 0.0, 1.0, 0.0);
	}
	
	if (displayRobot)
	{
		
		glPushMatrix();
		glTranslatef(0, 1.0, 5.0);
		
		glRotatef(90, 0.0, 1.0, 0.0);
		
		drawRobot();
		
		glPopMatrix();
		

	}
	
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, 0.0, 5.0);
	if (ToggAxis)
	{

		axis();
	}
	
	
	if (visiblePath)
	{
		if (circlePath)
		{
			glPushMatrix();
			glRotatef(90, 1.0, 0.0, 0.0);
			glTranslatef(0, -5, 0);
			drawCircle(0, 0, 0, 5, 19);
			glPopMatrix();
		}
		if (linePath)
		{
			glRotatef(-45, 0.0, 0.0, 1.0);
			glPushMatrix;

			drawLine();
			glPopMatrix;
		}
	}
	
	
	

	glPopMatrix();
}

// Displays the arm in its current position and orientation. The whole
// function is bracketed by glPushMatrix and glPopMatrix calls because every
// time we call it we are in an "environment" in which a gluLookAt is in
// effect. (Note that in particular, replacing glPushMatrix with
// glLoadIdentity makes you lose the camera setting from gluLookAt).
void display(void) {

	//update the modelview matrix based on the camera's position
	glMatrixMode(GL_MODELVIEW); //make sure we aren't changing the projection matrix!
	glLoadIdentity();
	gluLookAt(x, y, z, //camera is located at (x,y,z)
		0, 0, 0, //camera is looking at (0,0,0)
		0.0f, 1.0f, 0.0f); //up vector is (0,1,0) (positive Y)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glScalef(0.3, 0.3, 0.3);
	drawScene();

	glutSwapBuffers();
}

// Handles the reshape event by setting the viewport so that it takes up the
// whole visible region, then sets the projection matrix to something reason-
// able that maintains proper aspect ratio.
void resizeWindow(int w, int h)
{
	aspectRatio = w / (float)h;
	windowWidth = w;
	windowHeight = h;
	//update the viewport to fill the window
	glViewport(0, 0, w, h);
	//update the projection matrix with the new window properties
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, aspectRatio, 0.1, 10);
}

// Perfroms application specific initialization: turn off smooth shading,
// sets the viewing transformation once and for all. In this application we
// won't be moving the camera at all, so it makes sense to do this.
void init() {

	glShadeModel(GL_FLAT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(1, 2, 8, 0, 0, 0, 0, 1, 0);
}



void procKeys(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1':
		displayWireMesh = true;
		displayRobot = true;
		break;
	case '2':
		displayWireMesh = false;
		displayRobot = true;
		break;
	case '3':
		ToggAxis = !ToggAxis;
		break;
	case '4':
		visiblePath = !visiblePath;
		break;
	case 'r':
		moveX = 0;
		spinAngle = 0;
		break;
	case 'a':
		playAnimation = !playAnimation;
		break;
	case 'p':
		linePath = !linePath;
		circlePath = !circlePath;
		break;

		//for the easter egg, we just disable and enable and reset values when we toggle the easteregg on and off, so that we do not have any bugs.
		//we enable shoulderZ movement when we enable the easter egg, which switches the direction of the shoulder movement to create a flying effect.
	case 'e':
		if (EasterEgg == true)
		{
			displayRobot = true;
			ToggAxis = true;
			aniSeconds = 0;
			aniState = 0;
			shoulderAngle = 0;
			hipAngle = 0;
			shoulderZ = 0;
			shoulderY = 1;
			moveY = 0;
			moveX = 0;
			EasterEgg = false;
			linePath = true;
			circlePath = false;
			playAnimation = true;
		}
		else
		{
			displayRobot = true;
			ToggAxis = false;
			displayWireMesh = false;
			
			shoulderY = 0;
			shoulderZ = 1;
			shoulderAngle = -45;
			hipAngle = 0;
			moveX = 0;
			moveY = 0;
			spinAngle = 0;
			playAnimation = false;
			EasterEgg = true;
			linePath = false;
			circlePath = false;
		}
		
		
		
		break;

	case 27: // escape
		exit(0);
	}
}

void special(int key, int, int)
{
	
	switch (key)
	{
	case GLUT_KEY_END: exit(0); break;
	defualt: return;
	}
}

void timer(int v)
{
	if (displayRobot)
	{
		if (playAnimation)
		{
			if (aniSeconds <= 30 && aniState == 0)
			{
				shoulderAngle += 1;
				hipAngle += 1;

				aniSeconds++;
				if (aniSeconds >= 30)
					aniState = 1;
			}
			else
			{
				shoulderAngle -= 1;
				hipAngle -= 1;
				aniSeconds--;
				if (aniSeconds <= -30)
					aniState = 0;
			}


			if (EasterEgg == false)
			{
				// line
				if (moveX > -14 && linePath) {
					moveX = moveX - 0.07;
				}

				else {

					moveX = 0;
				}

				// circle
				if (circlePath)
				{
					spinAngle = spinAngle - 1;
				}
			}

		}
		//for the easter egg, I just changed the direction of the shoulder movement to be vertical instead of horizontal.
			if (EasterEgg)
			{
				if (moveY < 10 ) {
					moveX = moveX - 0.06;
					moveY = moveY + 0.06;
				}

				else {
					moveY = 0;
					moveX = 0;
				}

				if (aniSeconds <= 30 && aniState == 0)
				{
					shoulderAngle += 1;
					//hipAngle += 1;

					aniSeconds++;
					if (aniSeconds >= 30)
						aniState = 1;
				}
				else
				{
					shoulderAngle -= 1;
					//hipAngle -= 1;
					aniSeconds--;
					if (aniSeconds <= -30)
						aniState = 0;
				}

			}
			
		
	}

	glutPostRedisplay();
	glutTimerFunc(1000 / 60, timer, v);
}

// Initializes GLUT, the display mode, and main window; registers callbacks;
// does application initialization; enters the main event loop.
int main(int argc, char** argv) {
	//movement();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(80, 80);
	glutInitWindowSize(800, 600);
	glutCreateWindow("(Karim Amr Mohamed Talaat Mohamed Hassan) (811238841)");

	printf("\n\
----------------------------------------------------------------------------\n\
Key Instructions:\n\
- '1': display a wireframe (mesh only) model  \n\
- '2': display a solid model   \n\
- '3': toggle on/off to draw axes \n\
- '4' : robot walking path visible toggle ON/OFF  \n\
- 'r' : move the robot to the initial position to be animated  \n\
- 'a' : animation walking toggle ON/OFF (animation only)  \n\
- 'p' : alking path options of the robot (at least two; e.g., 1 time key pressing – line segment, 2 times key pressing – circle)   \n\
- 'e': toggle on/off EasterEgg \n\
- ESC to quit \n\
----------------------------------------------------------------------------\n");

	// Init depth checking
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);

	cameraRadius = 7.0f;
	cameraTheta = 2.80;
	cameraPhi = 1.0;
	recomputeOrientation();

	// Register callbacks
	glutDisplayFunc(display);
	glutTimerFunc(0, timer, 0);
	glutReshapeFunc(resizeWindow);
	glutMouseFunc(mouseCallback);
	glutMotionFunc(mouseMotion);
	glutKeyboardFunc(procKeys);
	glutSpecialFunc(special);
	init();
	glutMainLoop();
}