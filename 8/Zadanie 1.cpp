// Zadanie 1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <GL/glut.h>
#include <stdlib.h>
#include "colors.h"

// sta�e do obs�ugi menu podr�cznego
enum
{
	WARIANT1,
	WARIANT2,
	WARIANT3,
	FULL_WINDOW, // aspekt obrazu - ca�e okno
	ASPECT_1_1, // aspekt obrazu 1:1
	EXIT // wyj�cie
};

// aspekt obrazu

int aspect = FULL_WINDOW;

// rozmiary bry�y obcinania

const GLdouble left = -2.0;
const GLdouble right = 2.0;
const GLdouble bottom = -2.0;
const GLdouble top = 2.0;
const GLdouble near_ = 3.0;
const GLdouble far_ = 7.0;

// k�ty obrotu sceny

GLfloat rotatex = 0.0;
GLfloat rotatey = 0.0;

// wska�nik naci�ni�cia lewego przycisku myszki

int button_state = GLUT_UP;

// po�o�enie kursora myszki

int button_x, button_y;

// identyfikatory list wy�wietlania

GLint Kula, Kula2, Torus, Ostroslup, Czworoscian, Dwudziestoscian;

// rodzaj operacji CSG

int csg_op = WARIANT1;

// ustawienie bufora szablonowego tak, aby wydzieli� i wy�wietli�
// te elementy obiektu A, kt�re znajduj� si� we wn�trzu obiektu B;
// stron� (przedni� lub tyln�) wyszukiwanych element�w obiektu A
// okre�la parametr cull_face

void Inside(GLint A, GLint B, GLenum cull_face, GLenum stencil_func)
{
	// pocz�tkowo rysujemy obiekt A w buforze g��boko�ci przy
	// wy��czonym zapisie sk�adowych RGBA do bufora kolor�w

	// w��czenie testu bufora g��boko�ci
	glEnable(GL_DEPTH_TEST);

	// wy��czenie zapisu sk�adowych RGBA do bufora kolor�w
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	// rysowanie wybranej strony wielok�t�w
	glCullFace(cull_face);

	// wy�wietlenie obiektu A
	glCallList(A);

	// nast�pnie przy u�yciu bufora szablonowego wykrywamy te elementy
	// obiektu A, kt�re znajduj� si� wewn�trz obiektu B; w tym celu
	// zawarto�� bufora szablonowego jest zwi�kszana o 1, wsz�dzie gdzie
	// b�d� przednie strony wielok�t�w sk�adaj�cych si� na obiekt B

	// wy��czenie zapisu do bufora g��boko�ci
	glDepthMask(GL_FALSE);

	// w��czenie bufora szablonowego
	glEnable(GL_STENCIL_TEST);

	// test bufora szablonowego
	glStencilFunc(GL_ALWAYS, 0, 0);

	// okre�lenie operacji na buforze szablonowym
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

	// rysowanie tylko przedniej strony wielok�t�w
	glCullFace(GL_BACK);

	// wy�wietlenie obiektu B
	glCallList(B);

	// w kolejnym etapie zmniejszamy zawarto�� bufora szablonowego o 1
	// wsz�dzie tam, gdzie s� tylne strony wielok�t�w obiektu B

	// okre�lenie operacji na buforze szablonowym
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);

	// rysowanie tylko tylnej strony wielok�t�w
	glCullFace(GL_FRONT);

	// wy�wietlenie obiektu B
	glCallList(B);

	// dalej wy�wietlamy te elementy obiektu A, kt�re
	// znajduj� si� we wn�trzu obiektu B

	// w��czenie zapisu do bufora g��boko�ci
	glDepthMask(GL_TRUE);

	// w��czenie zapisu sk�adowych RGBA do bufora kolor�w
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	// test bufora szablonowego
	glStencilFunc(stencil_func, 0, 1);

	// wy��czenie testu bufora g��boko�ci
	glDisable(GL_DEPTH_TEST);

	// rysowanie wybranej strony wielok�t�w
	glCullFace(cull_face);

	// wy�wietlenie obiektu A
	glCallList(A);

	// wy��czenie bufora szablonowego
	glDisable(GL_STENCIL_TEST);
}


void AND(GLint o1, GLint o2)
{
	// elementy obiektu A znajduj�ce si� we wn�trzu B
	Inside(o1, o2, GL_BACK, GL_NOTEQUAL);

	// wy��czenie zapisu sk�adowych RGBA do bufora kolor�w
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	// w��czenie testu bufora g��boko�ci
	glEnable(GL_DEPTH_TEST);

	// wy��czenie bufora szablonowego
	glDisable(GL_STENCIL_TEST);

	// wyb�r funkcji do testu bufora g��boko�ci
	glDepthFunc(GL_ALWAYS);

	// wy�wietlenie obiektu B
	glCallList(o2);

	// wyb�r funkcji do testu bufora g��boko�ci
	glDepthFunc(GL_LESS);

	// elementy obiektu B znajduj�ce si� we wn�trzu A
	Inside(o2, o1, GL_BACK, GL_NOTEQUAL);

	glDisable(GL_DEPTH_TEST);
}

void SUB(GLint o1, GLint o2)
{
	// elementy obiektu A znajduj�ce si� we wn�trzu B
	Inside(o1, o2, GL_FRONT, GL_NOTEQUAL);

	// wy��czenie zapisu sk�adowych RGBA do bufora kolor�w
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	// w��czenie testu bufora g��boko�ci
	glEnable(GL_DEPTH_TEST);

	// wy��czenie bufora szablonowego
	glDisable(GL_STENCIL_TEST);

	// wyb�r funkcji do testu bufora g��boko�ci
	glDepthFunc(GL_ALWAYS);

	// wy�wietlenie obiektu B
	glCallList(o2);

	// wyb�r funkcji do testu bufora g��boko�ci
	glDepthFunc(GL_LESS);

	// elementy obiektu B znajduj�ce si� we wn�trzu A
	Inside(o2, o1, GL_BACK, GL_EQUAL);

	glDisable(GL_DEPTH_TEST);
}

// funkcja generuj�ca scen� 3D

void DisplayScene()
{
	// kolor t�a - zawarto�� bufora koloru
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// czyszczenie bufora koloru, bufora g��boko�ci i bufora szablonowego
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// wyb�r macierzy modelowania
	glMatrixMode(GL_MODELVIEW);

	// macierz modelowania = macierz jednostkowa
	glLoadIdentity();

	// przesuni�cie uk�adu wsp�rz�dnych obiekt�w do �rodka bry�y odcinania
	glTranslatef(0, 0, -(near_ + far_) / 2);

	// obroty ca�ej sceny
	glRotatef(rotatex, 1.0, 0.0, 0.0);
	glRotatef(rotatey, 0.0, 1.0, 0.0);

	// w��czenie o�wietlenia
	glEnable(GL_LIGHTING);

	// w��czenie �wiat�a GL_LIGHT0
	glEnable(GL_LIGHT0);

	// w��czenie automatycznej normalizacji wektor�w normalnych
	glEnable(GL_NORMALIZE);

	// w��czenie obs�ugi w�a�ciwo�ci materia��w
	glEnable(GL_COLOR_MATERIAL);

	// w�a�ciwo�ci materia�u okre�lone przez kolor wierzcho�k�w
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// w��czenie rysowania wybranej strony wielok�t�w
	glEnable(GL_CULL_FACE);

	// v1
	if (csg_op == WARIANT1)
	{
		// w��czenie testu bufora g��boko�ci
		glEnable(GL_DEPTH_TEST);

		SUB(Ostroslup, Kula);
		glClear(GL_STENCIL_BUFFER_BIT);
		AND(Torus, Kula);

		// wy��czenie testu bufora g��boko�ci
		glDisable(GL_DEPTH_TEST);
	}
	// v2
	if (csg_op == WARIANT2)
	{
		glEnable(GL_DEPTH_TEST);

		SUB(Ostroslup, Czworoscian);
		glClear(GL_STENCIL_BUFFER_BIT);
		AND(Kula, Torus);

		glDisable(GL_DEPTH_TEST);
	}

	// v3
	if (csg_op == WARIANT3)
	{
		glEnable(GL_DEPTH_TEST);

		SUB(Ostroslup, Kula);
		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		AND(Dwudziestoscian, Kula2);

		glDisable(GL_DEPTH_TEST);
	}

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
			glFrustum(left, right, bottom * height / width, top * height / width, near_, far_);
		else

			// szeroko�� okna wi�ksza lub r�wna wysoko�ci okna
		if (width >= height && height > 0)
			glFrustum(left * width / height, right * width / height, bottom, top, near_, far_);

	}
	else
		glFrustum(left, right, bottom, top, near_, far_);

	// generowanie sceny 3D
	DisplayScene();
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
		// operacja CSG
	case WARIANT1:
	case WARIANT2:
	case WARIANT3:
		csg_op = value;
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

// utworzenie list wy�wietlania

void GenerateDisplayLists()
{
	Kula = glGenLists(1);

	glNewList(Kula, GL_COMPILE);

	glColor4fv(Green);
	glutSolidSphere(0.6, 30, 30);

	glEndList();


	Kula2 = glGenLists(1);

	glNewList(Kula2, GL_COMPILE);

	glColor4fv(Blue);
	glutSolidSphere(0.9, 30, 30);

	glEndList();


	Torus = glGenLists(1);

	glNewList(Torus, GL_COMPILE);

	glColor4fv(Blue);
	glutSolidTorus(0.1, 0.6, 10, 20);

	glEndList();


	Ostroslup = glGenLists(1);

	glNewList(Ostroslup, GL_COMPILE);

	glBegin(GL_TRIANGLES);
	glColor3fv(Red);
	glVertex3f(1.0, -1.0, -1.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f(0.0, 1.0, 0.0);

	glColor3fv(Blue);
	glVertex3f(0.0, -1.0, 1.0);
	glVertex3f(1.0, -1.0, -1.0);
	glVertex3f(0.0, 1.0, 0.0);

	glColor3fv(Green);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f(0.0, -1.0, 1.0);
	glVertex3f(0.0, 1.0, 0.0);

	glColor3fv(Yellow);
	glVertex3f(1.0, -1.0, -1.0);
	glVertex3f(0.0, -1.0, 1.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glEnd();

	glEndList();

	Czworoscian = glGenLists(1);
	glNewList(Czworoscian, GL_COMPILE);

	glColor4fv(Brown);
	glutSolidTetrahedron();
	glEndList();

	Dwudziestoscian = glGenLists(1);
	glNewList(Dwudziestoscian, GL_COMPILE);

	glColor4fv(Purple);
	glutSolidIcosahedron();
	glEndList();
}

int main(int argc, char * argv[])
{
	// inicjalizacja biblioteki GLUT
	glutInit(&argc, argv);

	// inicjalizacja bufora ramki
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);

	// rozmiary g��wnego okna programu
	glutInitWindowSize(500, 500);

	// utworzenie g��wnego okna programu
	glutCreateWindow("CSG");

	// do��czenie funkcji generuj�cej scen� 3D
	glutDisplayFunc(DisplayScene);

	// do��czenie funkcji wywo�ywanej przy zmianie rozmiaru okna
	glutReshapeFunc(Reshape);

	// obs�uga przycisk�w myszki
	glutMouseFunc(MouseButton);

	// obs�uga ruchu kursora myszki
	glutMotionFunc(MouseMotion);

	// utworzenie podmenu - Operacja CSG
	int MenuCSGOp = glutCreateMenu(Menu);
	glutAddMenuEntry("Wariant 1", WARIANT1);
	glutAddMenuEntry("Wariant 2", WARIANT2);
	glutAddMenuEntry("Wariant 3", WARIANT3);

	// utworzenie podmenu - Aspekt obrazu
	int MenuAspect = glutCreateMenu(Menu);
	glutAddMenuEntry("Aspekt obrazu - ca�e okno", FULL_WINDOW);
	glutAddMenuEntry("Aspekt obrazu 1:1", ASPECT_1_1);

	// menu g��wne
	glutCreateMenu(Menu);
	glutAddSubMenu("Operacja CSG", MenuCSGOp);

	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyj�cie", EXIT);


	// okre�lenie przycisku myszki obs�uguj�cego menu podr�czne
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// utworzenie list wy�wietlania
	GenerateDisplayLists();

	// wprowadzenie programu do obs�ugi p�tli komunikat�w
	glutMainLoop();
	return 0;
}