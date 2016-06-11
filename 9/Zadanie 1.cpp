﻿// Zadanie 1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

/*
* (c) Copyright 2016, Vasyl Martsenyuk
* Email: marceniuk@yahoo.com
*/

#include <windows.h> // plik nagłówkowy dla Windows
#include <math.h> // plik nagłówkowy dla biblioteki matematycznej
#include <stdio.h> // plik nagłówkowy dla we/wy standardowego
#include <stdlib.h> // plik nagłówkowy dla bibioteki standardowej
#include <gl\gl.h> // plik nagłówkowy dla biblioteki OpenGL32
#include <gl\glu.h> // plik nagłówkowy dla biblioteki GLu32
#include <GL\glut.h> // plik nagłówkowy dla biblioteki Glut
#include "glext.h"
#include "targa.h"
#include "stdio.h"

// stałe do obsługi menu podręcznego
enum
{
	MARS_TEX, 
	WATER_TEX,
	STONE_TEX
};

GLint texture = MARS_TEX;
GLdouble Time = 1000;

// rozmiary bryły obcinania
const GLdouble left = -3.0;
const GLdouble right = 3.0;
const GLdouble bottom = -3.0;
const GLdouble top = 3.0;
const GLdouble near_ = -6.0;
const GLdouble far_ = 6.0;

// kąty obrotu
GLfloat rotatex = 0.0;
GLfloat rotatez = 0.0;

// wskaźnik naciśnięcia lewego przycisku myszki
int button_state = GLUT_UP;

// położenie kursora myszki
int button_x, button_y;

// współczynnik skalowania
GLfloat scale = 3.5;

typedef struct point_3d // struktura dla punktu 3D
{  
	double x, y, z;
} POINT_3D;

typedef struct bpatch // struktura dla wielomiana fragmentu Beziera 3 stopnia
{ 
	POINT_3D anchors[4][6]; // siatka 4x6 punktów kontrolnych (anchor)
	GLuint dlBPatch; // lista dla fragmentu Beziera
	GLuint texture; // tekstura dla fragmentu
} BEZIER_PATCH;

HDC hDC = NULL; // kontekst urządzenia
HGLRC hRC = NULL; // kontekst renderingu
HWND hWnd = NULL; // deskryptor okna
HINSTANCE hInstance; // egzemplarz programu

bool keys[256]; // tablica jednowymiarowa dla klawiatury
bool active = TRUE; // flag of application activity
bool fullscreen = TRUE; // flag of full-screen mode

DEVMODE DMsaved; // zachować ustawienia przeszłego trybu

GLfloat rotz = 0.0f; // obroty współrzędnej Z
BEZIER_PATCH mybezier; // fragment Beziera dla wykorzystania
bool showCPoints = TRUE; // switcher of anchor points of net
int divs = 7; // number of interpolation (distribution of polygon)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // deklaracja dla WndProc

//functions for vector operations. It can be used any C++ class for 3D point

// addition of 2 points

POINT_3D pointAdd(POINT_3D p, POINT_3D q) 
{
	p.x += q.x; p.y += q.y; p.z += q.z;
	return p;
}

// multiplication of point by constant

POINT_3D pointTimes(double c, POINT_3D p) 
{
	p.x *= c; p.y *= c; p.z *= c;
	return p;
}

// making new point

POINT_3D makePoint(double a, double b, double c) 
{
	POINT_3D p;
	p.x = a; p.y = b; p.z = c;
	return p;
}

POINT_3D SinFunction(POINT_3D &pkt, GLdouble &t = Time, GLint n = 6)
{
	pkt.x *= sin(n*1.e-4*(t += 0.5));
	pkt.y *= sin(n*1.e-4*(t += 0.5));
	pkt.z *= sin(n*1.e-4*(t += 0.5));

	return pkt;
}

POINT_3D Bernstein(float u, POINT_3D *p) {

	POINT_3D a, b, c, d, r;

	a = pointTimes(pow(u, 3), p[0]);
	b = pointTimes(3 * pow(u, 2)*(1 - u), p[1]);
	c = pointTimes(3 * u*pow((1 - u), 2), p[2]);
	d = pointTimes(pow((1 - u), 3), p[3]);

	r = pointAdd(pointAdd(a, b), pointAdd(c, d));

	return r;
}


GLuint genBezier(BEZIER_PATCH patch, int divs) {

	int u = 0, v;
	float py, px, pyold;
	GLuint drawlist = glGenLists(1);
	POINT_3D temp[4];
	POINT_3D *last = (POINT_3D*)malloc(sizeof(POINT_3D)*(divs + 1));

	if (patch.dlBPatch != NULL)
		glDeleteLists(patch.dlBPatch, 1);


	temp[0] = patch.anchors[0][5];
	temp[1] = patch.anchors[1][5];
	temp[2] = patch.anchors[2][5];
	temp[3] = patch.anchors[3][5];


	for (v = 0; v <= divs; v++) 
	{		
		px = ((float)v) / ((float)divs);
		last[v] = Bernstein(px, temp);
	}

	glNewList(drawlist, GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, patch.texture);


	for (u = 1; u <= divs; u++) 
	{
		py = ((float)u) / ((float)divs);
		pyold = ((float)u - 1.0f) / ((float)divs);


		temp[0] = Bernstein(py, patch.anchors[0]);
		temp[1] = Bernstein(py, patch.anchors[1]);
		temp[2] = Bernstein(py, patch.anchors[2]);
		temp[3] = Bernstein(py, patch.anchors[3]);

		glBegin(GL_TRIANGLE_STRIP);

		for (v = 0; v <= divs; v++) 
		{
			px = ((float)v) / ((float)divs);

			glTexCoord2f(pyold, px);
			glVertex3d(last[v].x, last[v].y, last[v].z);

			last[v] = Bernstein(px, temp);
			glTexCoord2f(py, px);
			glVertex3d(last[v].x, last[v].y, last[v].z);
		}
		glEnd();
	}

	glEndList();
	free(last);
	return drawlist; 
}


void initBezier(void) {

	mybezier.anchors[0][0] = SinFunction(makePoint(-0.75, -0.75, -0.5));
	mybezier.anchors[0][1] = SinFunction(makePoint(-0.25, -0.75, 0.00));
	mybezier.anchors[0][2] = SinFunction(makePoint(0.25, -0.75, 0.00));
	mybezier.anchors[0][3] = SinFunction(makePoint(0.75, -0.75, -0.50));
	mybezier.anchors[0][4] = SinFunction(makePoint(0.8, -0.75, -0.3));
	mybezier.anchors[0][5] = SinFunction(makePoint(0.85, -0.9, -0.1));

	mybezier.anchors[1][0] = SinFunction(makePoint(-0.75, -0.25, -0.75));
	mybezier.anchors[1][1] = SinFunction(makePoint(-0.25, -0.25, 0.50));
	mybezier.anchors[1][2] = SinFunction(makePoint(0.25, -0.25, 0.50));
	mybezier.anchors[1][3] = SinFunction(makePoint(0.75, -0.25, -0.75));
	mybezier.anchors[1][4] = SinFunction(makePoint(0.8, -0.25, -0.8));
	mybezier.anchors[1][5] = SinFunction(makePoint(0.9, -0.25, -0.9));

	mybezier.anchors[2][0] = SinFunction(makePoint(-0.75, 0.25, 0.00));
	mybezier.anchors[2][1] = SinFunction(makePoint(-0.25, 0.25, -0.50));
	mybezier.anchors[2][2] = SinFunction(makePoint(0.25, 0.25, -0.50));
	mybezier.anchors[2][3] = SinFunction(makePoint(0.75, 0.25, 0.00));
	mybezier.anchors[2][4] = SinFunction(makePoint(0.8, 0.25, 0.25));
	mybezier.anchors[2][5] = SinFunction(makePoint(0.9, 0.5, 0.4));

	mybezier.anchors[3][0] = SinFunction(makePoint(-0.75, 0.75, -0.50));
	mybezier.anchors[3][1] = SinFunction(makePoint(-0.25, 0.75, -1.00));
	mybezier.anchors[3][2] = SinFunction(makePoint(0.25, 0.75, -1.00));
	mybezier.anchors[3][3] = SinFunction(makePoint(0.75, 0.75, -0.50));
	mybezier.anchors[3][4] = SinFunction(makePoint(0.8, 0.75, 0.1));
	mybezier.anchors[3][5] = SinFunction(makePoint(0.9, 0.75, 0.3));

	mybezier.dlBPatch = NULL;

}


int InitGL(GLvoid)
{
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glClearColor(1.00f, 1.00f, 1.00f, 0.5f); // Фон biały
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	initBezier();

	// wczytanie tekstur
	GLsizei width_texture, height_texture; // zmienne użyte przy obsłudze plików TARGA
	GLenum format, type;
	GLvoid *Ptr_mybezier_texture;
	GLboolean error;

	if (texture == MARS_TEX)
	{
		error = load_targa("ven0aaa2.tga", width_texture, height_texture, format, type, Ptr_mybezier_texture);
		if (error == GL_FALSE)
		{
			printf("Niepoprawny odczyt pliku");
			exit(0);
		}
	}

	if (texture == WATER_TEX)
	{
		error = load_targa("water_1.tga", width_texture, height_texture, format, type, Ptr_mybezier_texture);
		if (error == GL_FALSE)
		{
			printf("Niepoprawny odczyt pliku");
			exit(0);
		}
	}

	if (texture == STONE_TEX)
	{
		error = load_targa("stone_1.tga", width_texture, height_texture, format, type, Ptr_mybezier_texture);
		if (error == GL_FALSE)
		{
			printf("Niepoprawny odczyt pliku");
			exit(0);
		}
	}

	// dowiązanie stanu tekstury
	glBindTexture(GL_TEXTURE_2D, (GLuint)Ptr_mybezier_texture);

	// utworzenie tekstury wraz z mipmapami
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width_texture, height_texture, format, type, Ptr_mybezier_texture);

	mybezier.texture = (GLuint)Ptr_mybezier_texture;
	mybezier.dlBPatch = genBezier(mybezier, divs);

	// porządki
	delete[](GLvoid*)Ptr_mybezier_texture;

	return TRUE;
}

void DrawGLScene(GLvoid) {

	int i, j;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// obroty obiektu
	glRotatef(-75.0f, 1.0f, 0.0f, 0.0f);

	// przesunięcie układu współrzędnych obiektów do środka bryły odcinania
	glTranslatef(0.0, 0.0, -(near_ + far_) / 2);

	// obroty obiektu
	glRotatef(rotatex, 1.0, 0.0, 0.0);
	glRotatef(rotatez, 0.0, 0.0, 1.0);

	// skalowanie obiektu - klawisze "+" i "-"
	glScalef(scale, scale, scale);

	glCallList(mybezier.dlBPatch);

	BOOL showCPoint = TRUE;
	if (showCPoints)
	{
		glDisable(GL_TEXTURE_2D);
		glColor3f(0.0f, 1.0f, 0.0f);

		// rozmiar punktów
		glPointSize(6.0);

		for (i = 0; i < 4; i++)
		{
			glBegin(GL_LINE_STRIP);
			for (j = 0; j < 6; j++)
			{
				glVertex3d(mybezier.anchors[i][j].x, mybezier.anchors[i][j].y, mybezier.anchors[i][j].z);

			}
			glEnd();
		}

		for (i = 0; i < 6; i++)
		{
			glBegin(GL_LINE_STRIP);
			for (j = 0; j < 4; j++)
			{
				glVertex3d(mybezier.anchors[j][i].x, mybezier.anchors[j][i].y, mybezier.anchors[j][i].z);
			}
			glEnd();
		}

		glColor3f(1.0f, 1.0f, 1.0f);
		glEnable(GL_TEXTURE_2D);
	}

	// wyświetlenie sceny
	glutPostRedisplay();

	//return TRUE;
	glFlush();
	glutSwapBuffers();
}

void TimerFunction(int value) {
	InitGL();
	// Ponowne rysowanie sceny z nowymi współrzędnymi  
	glutPostRedisplay();
	glutTimerFunc(10, TimerFunction, 1);
}

void resize(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-5, 5, -5, 5, 2, 12);
	gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);
	glMatrixMode(GL_MODELVIEW);
	DrawGLScene();
}


void Menu(int value)
{
	switch (value)
	{
		// Mars
	case MARS_TEX:
		texture = MARS_TEX;
		InitGL();
		DrawGLScene();
		break;

		// Woda
	case WATER_TEX:
		texture = WATER_TEX;
		InitGL();
		DrawGLScene();
		break;

		// Kamień
	case STONE_TEX:
		texture = STONE_TEX;
		InitGL();
		DrawGLScene();
		break;
	}
}


// obsługa przycisków myszki
void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		// zapamiętanie stanu lewego przycisku myszki
		button_state = state;

		// zapamiętanie położenia kursora myszki
		if (state == GLUT_DOWN)
		{
			button_x = x;
			button_y = y;
		}
	}
}

// obsługa ruchu kursora myszki

void MouseMotion(int x, int y)
{
	if (button_state == GLUT_DOWN)
	{
		rotatez += 30 * (right - left) / glutGet(GLUT_WINDOW_WIDTH) * (x - button_x);
		button_x = x;
		rotatex -= 30 * (top - bottom) / glutGet(GLUT_WINDOW_HEIGHT) * (button_y - y);
		button_y = y;
		glutPostRedisplay();
	}
}

void Keyboard(unsigned char key, int x, int y)
{
	// klawisz +
	if (key == '+')
		scale += 0.05;
	else

		// klawisz -
	if (key == '-' && scale > 0.05)
		scale -= 0.05;

	// narysowanie sceny
	DrawGLScene();
}

int main(int argc, char *argv[])
{
	// inicjalizacja biblioteki GLUT
	glutInit(&argc, argv);

	// inicjalizacja bufora ramki
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowPosition(50, 10);

	// rozmiary głównego okna programu
	glutInitWindowSize(500, 500);

	// utworzenie głównego okna programu
	glutCreateWindow("Krzywa Beziera");

	// dołączenie funkcji generującej scenę 3D
	glutDisplayFunc(DrawGLScene);

	// dołączenie funkcji wywoływanej przy zmianie rozmiaru okna
	glutReshapeFunc(resize);
	//glutReshapeFunc(Reshape);

	// dołączenie funkcji obsługi klawiatury
	glutKeyboardFunc(Keyboard);

	// obsługa przycisków myszki
	glutMouseFunc(MouseButton);

	// obsługa ruchu kursora myszki
	glutMotionFunc(MouseMotion);

	glutCreateMenu(Menu);
	glutAddMenuEntry("Mars", MARS_TEX);
	glutAddMenuEntry("Woda", WATER_TEX);
	glutAddMenuEntry("Skala", STONE_TEX);

	// określenie przycisku myszki obsługującego menu podręczne
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	InitGL();

	glutTimerFunc(33, TimerFunction, 1);
	// wprowadzenie programu do obsługi pętli komunikatów
	glutMainLoop();

	// porządki
	delete[](GLvoid*)mybezier.texture;

	return 0;
}