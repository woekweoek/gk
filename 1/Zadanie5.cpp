// Zadanie5.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <GL/glut.h>
#include <vector>
using namespace std;

int numb = -1;

struct vect
{
	GLint x;
	GLint y;
	vect(GLint x, GLint y) : x(x), y(y) {}
};

vector<vect> v;

void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POINTS);
	for (int n = 0; n < v.size(); n++)
		glVertex2f(v[n].x, v[n].y);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);
	if (numb != 0 && v.size() >= numb)
	{
		glBegin(GL_POLYGON);
		for (int n = 0; n < numb; n++)
			glVertex2f(v[n].x, v[n].y);
		glEnd();
	}

	glFlush();
}

void SetupRC(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glPointSize(3.0f);
}

void ChangeSize(int w, int h)  
{
	GLfloat aspectRatio;

	if (h == 0)   h = 1;

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	aspectRatio = (GLfloat)w / (GLfloat)h;

	if (w <= h)
	{
		glOrtho(0.0f, w, 0.0f / aspectRatio, h / aspectRatio, 1.0f, -1.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glScalef(1.0f, (GLfloat)h / (GLfloat)w, 1.0f);
	}
	else
	{
		glOrtho(0.0 * aspectRatio, w * aspectRatio, 0.0f, h, 1.0f, -1.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glScalef((GLfloat)w / (GLfloat)h, 1.0f, 1.0f);
	}

}

void MouseDown(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		v.push_back(vect(x, glutGet(GLUT_WINDOW_HEIGHT) - y));
	}

}

enum MENU
{
	Tr,
	Cz,
	Pi,
	Sz,
	Si,
};

void menu(int item)
{
	switch (item)
	{
	case Tr: numb = 3; break;
	case Cz: numb = 4; break;
	case Pi: numb = 5; break;
	case Sz: numb = 6; break;
	case Si: numb = 7; break;
	default: break;
	}
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Zadanie 5");

	glutCreateMenu(menu);
	glutAddMenuEntry("Trojkat", Tr);
	glutAddMenuEntry("Czworokat", Cz);
	glutAddMenuEntry("Pieciokat", Pi);
	glutAddMenuEntry("Szesciokat", Sz);
	glutAddMenuEntry("Siedmiokat", Si);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	glutMouseFunc(MouseDown);

	SetupRC();
	glutMainLoop();
	return 0;
}

