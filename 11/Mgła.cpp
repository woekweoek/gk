// Zadanie 2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <GL/glut.h>
#include "glext.h"
#include <stdio.h>
#include <stdlib.h>
#include "colors.h"
#include <math.h>
#define GL_PI 3.1415

// wska�nik na funkcj� glWindowPos2i

PFNGLWINDOWPOS2IPROC glWindowPos2i = NULL;

// sta�e do obs�ugi menu podr�cznego

enum
{
	FULL_WINDOW = GL_ALWAYS + 100, // aspekt obrazu - ca�e okno
	ASPECT_1_1, // aspekt obrazu 1:1
	EXIT // wyj�cie
};

// aspekt obrazu

int aspect = FULL_WINDOW;

// usuni�cie definicji makr near i far

#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif

// rozmiary bry�y obcinania

const GLdouble left = -2.0;
const GLdouble right = 2.0;
const GLdouble bottom = -1.5;
const GLdouble top = 2.5;
const GLdouble near = 3.0;
const GLdouble far = 7.0;

// k�ty obrotu

GLfloat rotatex = 0.0;
GLfloat rotatey = 0.0;

// wska�nik naci�ni�cia lewego przycisku myszki

int button_state = GLUT_UP;

// po�o�enie kursora myszki

int button_x, button_y;

// identyfikator listy wy�wietlania

GLuint CUBE_LIST;

// wskaz�wki jako�ci generacji mg�y

GLint fog_hint = GL_DONT_CARE;

// pocz�tek i koniec oddzia�ywania mg�y liniowej

GLfloat fog_start = 3.0;
GLfloat fog_end = 5.0;

// g�sto�� mg�y

GLfloat fog_density = 0.5;

// rodzaj mg�y

GLfloat fog_mode = GL_LINEAR;

// funkcja rysuj�ca napis w wybranym miejscu
// (wersja korzystaj�ca z funkcji glWindowPos2i)

void DrawString(GLint x, GLint y, char * string)
{
	// po�o�enie napisu
	glWindowPos2i(x, y);

	// wy�wietlenie napisu
	int len = strlen(string);
	for (int i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, string[i]);

}

void DrawFigure()
{
	int i = 0;
	float x, y, angle;
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.0, 1.0, 1.0);
	glVertex3f(0.0f, 0.0f, 2.0f);
	for (angle = 0.0f; angle < (2.2f*GL_PI); angle += (GL_PI / 6.0f))
	{
		if (i % 2 == 0) glColor3f(0.0, 1.0, 1.0);
		else glColor3f(0.0, 0.0, 1.0);
		x = sin(angle);
		y = cos(angle);
		glVertex2f(x, y);
		i++;
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0.0f, 0.0f);
	for (angle = 0.0f; angle < (2.2f*GL_PI); angle += (GL_PI / 6.0f))
	{
		if (i % 2 == 0) glColor3f(0.0, 1.0, 1.0);
		else glColor3f(0.0, 0.0, 1.0);
		x = sin(angle);
		y = cos(angle);
		glVertex2f(x, y);
		i++;
	}
	glEnd();
	glPopMatrix();
}

// funkcja generuj�ca scen� 3D

void DisplayScene()
{
	// kolor t�a - zawarto�� bufora koloru
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// czyszczenie bufora koloru i bufora g��boko�ci
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// wyb�r macierzy modelowania
	glMatrixMode(GL_MODELVIEW);

	// macierz modelowania = macierz jednostkowa
	glLoadIdentity();

	// przesuni�cie uk�adu wsp�rz�dnych sze�cianu do �rodka bry�y odcinania
	glTranslatef(0.0, 0.0, -(near + far) / 2);

	// obroty sze�cianu
	glRotatef(rotatex, 1.0, 0.0, 0.0);
	glRotatef(rotatey, 0.0, 1.0, 0.0);

	// niewielkie powi�kszenie sze�cianu
	glScalef(1.15, 1.15, 1.15);

	// w��czenie testu bufora g��boko�ci
	glEnable(GL_DEPTH_TEST);

	// w��czenie efektu mg�y
	glEnable(GL_FOG);

	// wskaz�wki jako�ci generacji mg�y
	glHint(GL_FOG_HINT, fog_hint);

	// kolor mg�y
	glFogfv(GL_FOG_COLOR, White);

	// g�sto�� mg�y
	glFogf(GL_FOG_DENSITY, fog_density);

	// rodzaj mg�y
	glFogf(GL_FOG_MODE, fog_mode);

	// pocz�tek i koniec oddzia�ywania mg�y liniowej
	glFogf(GL_FOG_START, fog_start);
	glFogf(GL_FOG_END, fog_end);

	// wy�wietlenie sze�cianu
	//glCallList(CUBE_LIST);

	DrawFigure();

	// wy��czenie efektu mg�y
	glDisable(GL_FOG);

	// skierowanie polece� do wykonania
	glFlush();

	// zamiana bufor�w koloru
	glutSwapBuffers();
}

// zmiana wielko�ci okna

void Reshape(int width, int height)
{
	// obszar renderingu - ca�e okno
	glViewport(0, 0, width, height);

	// wyb�r macierzy rzutowania
	glMatrixMode(GL_PROJECTION);

	// macierz rzutowania = macierz jednostkowa
	glLoadIdentity();

	// parametry bry�y obcinania
	if (aspect == ASPECT_1_1)
	{
		// wysoko�� okna wi�ksza od wysoko�ci okna
		if (width < height && width > 0)
			glFrustum(left, right, bottom * height / width, top * height / width, near, far);
		else

			// szeroko�� okna wi�ksza lub r�wna wysoko�ci okna
		if (width >= height && height > 0)
			glFrustum(left * width / height, right * width / height, bottom, top, near, far);

	}
	else
		glFrustum(left, right, bottom, top, near, far);

	// generowanie sceny 3D
	DisplayScene();
}

// obs�uga klawiatury

void Keyboard(unsigned char key, int x, int y)
{
	// klawisz +
	if (key == '+')
		fog_density += 0.05;
	else

		// klawisz -
	if (key == '-' && fog_density > 0.05)
		fog_density -= 0.05;

	// narysowanie sceny
	DisplayScene();
}

// obs�uga klawiszy funkcyjnych i klawiszy kursora

void SpecialKeys(int key, int x, int y)
{
	switch (key)
	{
		// kursor w g�r�
	case GLUT_KEY_UP:
		fog_start += 0.1;
		fog_end += 0.1;
		break;

		// kursor w d�
	case GLUT_KEY_DOWN:
		fog_start -= 0.1;
		fog_end -= 0.1;
		break;
	}

	// odrysowanie okna
	Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

// obs�uga przycisk�w myszki

void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		// zapami�tanie stanu lewego przycisku myszki
		button_state = state;

		// zapami�tanie po�o�enia kursora myszki
		if (state == GLUT_DOWN)
		{
			button_x = x;
			button_y = y;
		}
	}
}

// obs�uga ruchu kursora myszki

void MouseMotion(int x, int y)
{
	if (button_state == GLUT_DOWN)
	{
		rotatey += 30 * (right - left) / glutGet(GLUT_WINDOW_WIDTH) *(x - button_x);
		button_x = x;
		rotatex -= 30 * (top - bottom) / glutGet(GLUT_WINDOW_HEIGHT) *(button_y - y);
		button_y = y;
		glutPostRedisplay();
	}
}

// obs�uga menu podr�cznego

void Menu(int value)
{
	switch (value)
	{
		// rodzaj mg�y
	case GL_LINEAR:
	case GL_EXP:
	case GL_EXP2:
		fog_mode = value;
		DisplayScene();
		break;

		// GL_FOG_HINT
	case GL_FASTEST:
	case GL_DONT_CARE:
	case GL_NICEST:
		fog_hint = value;
		DisplayScene();
		break;

		// obszar renderingu - ca�e okno
	case FULL_WINDOW:
		aspect = FULL_WINDOW;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// obszar renderingu - aspekt 1:1
	case ASPECT_1_1:
		aspect = ASPECT_1_1;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// wyj�cie
	case EXIT:
		exit(0);
	}
}


int main(int argc, char * argv[])
{
	// inicjalizacja biblioteki GLUT
	glutInit(&argc, argv);

	// inicjalizacja bufora ramki
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// rozmiary g��wnego okna programu
	glutInitWindowSize(500, 500);

	// utworzenie g��wnego okna programu
#ifdef WIN32

	glutCreateWindow("Mg�a odleg�o�ciowa");
#else

	glutCreateWindow("Mgla odleglosciowa");
#endif

	// do��czenie funkcji generuj�cej scen� 3D
	glutDisplayFunc(DisplayScene);

	// do��czenie funkcji wywo�ywanej przy zmianie rozmiaru okna
	glutReshapeFunc(Reshape);

	// do��czenie funkcji obs�ugi klawiatury
	glutKeyboardFunc(Keyboard);

	// do��czenie funkcji obs�ugi klawiszy funkcyjnych i klawiszy kursora
	glutSpecialFunc(SpecialKeys);

	// obs�uga przycisk�w myszki
	glutMouseFunc(MouseButton);

	// obs�uga ruchu kursora myszki
	glutMotionFunc(MouseMotion);

	// utworzenie menu podr�cznego
	glutCreateMenu(Menu);

	// utworzenie podmenu - aspekt obrazu
	int MenuAspect = glutCreateMenu(Menu);
	glutAddMenuEntry("Aspekt obrazu - ca�e okno", FULL_WINDOW);
	glutAddMenuEntry("Aspekt obrazu 1:1", ASPECT_1_1);

	// utworzenie podmenu - Rodzaj mg�y
	int MenuFogMode = glutCreateMenu(Menu);
	glutAddMenuEntry("GL_LINEAR", GL_LINEAR);
	glutAddMenuEntry("GL_EXP", GL_EXP);
	glutAddMenuEntry("GL_EXP2", GL_EXP2);

	// utworzenie podmenu - GL_FOG_HINT
	int MenuFogHint = glutCreateMenu(Menu);
	glutAddMenuEntry("GL_FASTEST", GL_FASTEST);
	glutAddMenuEntry("GL_DONT_CARE", GL_DONT_CARE);
	glutAddMenuEntry("GL_NICEST", GL_NICEST);

	// menu g��wne
	glutCreateMenu(Menu);
	glutAddSubMenu("Rodzaj mg�y", MenuFogMode);
	glutAddSubMenu("GL_FOG_HINT", MenuFogHint);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyj�cie", EXIT);


	// okre�lenie przycisku myszki obs�uguj�cej menu podr�czne
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// wprowadzenie programu do obs�ugi p�tli komunikat�w
	glutMainLoop();
	return 0;
}