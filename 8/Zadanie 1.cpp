// Zadanie 1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <GL/glut.h>
#include <stdlib.h>
#include "colors.h"

// sta³e do obs³ugi menu podrêcznego
enum
{
	WARIANT1,
	WARIANT2,
	WARIANT3,
	FULL_WINDOW, // aspekt obrazu - ca³e okno
	ASPECT_1_1, // aspekt obrazu 1:1
	EXIT // wyjœcie
};

// aspekt obrazu

int aspect = FULL_WINDOW;

// rozmiary bry³y obcinania

const GLdouble left = -2.0;
const GLdouble right = 2.0;
const GLdouble bottom = -2.0;
const GLdouble top = 2.0;
const GLdouble near_ = 3.0;
const GLdouble far_ = 7.0;

// k¹ty obrotu sceny

GLfloat rotatex = 0.0;
GLfloat rotatey = 0.0;

// wskaŸnik naciœniêcia lewego przycisku myszki

int button_state = GLUT_UP;

// po³o¿enie kursora myszki

int button_x, button_y;

// identyfikatory list wyœwietlania

GLint Kula, Kula2, Torus, Ostroslup, Czworoscian, Dwudziestoscian;

// rodzaj operacji CSG

int csg_op = WARIANT1;

// ustawienie bufora szablonowego tak, aby wydzieliæ i wyœwietliæ
// te elementy obiektu A, które znajduj¹ siê we wnêtrzu obiektu B;
// stronê (przedni¹ lub tyln¹) wyszukiwanych elementów obiektu A
// okreœla parametr cull_face

void Inside(GLint A, GLint B, GLenum cull_face, GLenum stencil_func)
{
	// pocz¹tkowo rysujemy obiekt A w buforze g³êbokoœci przy
	// wy³¹czonym zapisie sk³adowych RGBA do bufora kolorów

	// w³¹czenie testu bufora g³êbokoœci
	glEnable(GL_DEPTH_TEST);

	// wy³¹czenie zapisu sk³adowych RGBA do bufora kolorów
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	// rysowanie wybranej strony wielok¹tów
	glCullFace(cull_face);

	// wyœwietlenie obiektu A
	glCallList(A);

	// nastêpnie przy u¿yciu bufora szablonowego wykrywamy te elementy
	// obiektu A, które znajduj¹ siê wewn¹trz obiektu B; w tym celu
	// zawartoœæ bufora szablonowego jest zwiêkszana o 1, wszêdzie gdzie
	// bêd¹ przednie strony wielok¹tów sk³adaj¹cych siê na obiekt B

	// wy³¹czenie zapisu do bufora g³êbokoœci
	glDepthMask(GL_FALSE);

	// w³¹czenie bufora szablonowego
	glEnable(GL_STENCIL_TEST);

	// test bufora szablonowego
	glStencilFunc(GL_ALWAYS, 0, 0);

	// okreœlenie operacji na buforze szablonowym
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

	// rysowanie tylko przedniej strony wielok¹tów
	glCullFace(GL_BACK);

	// wyœwietlenie obiektu B
	glCallList(B);

	// w kolejnym etapie zmniejszamy zawartoœæ bufora szablonowego o 1
	// wszêdzie tam, gdzie s¹ tylne strony wielok¹tów obiektu B

	// okreœlenie operacji na buforze szablonowym
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);

	// rysowanie tylko tylnej strony wielok¹tów
	glCullFace(GL_FRONT);

	// wyœwietlenie obiektu B
	glCallList(B);

	// dalej wyœwietlamy te elementy obiektu A, które
	// znajduj¹ siê we wnêtrzu obiektu B

	// w³¹czenie zapisu do bufora g³êbokoœci
	glDepthMask(GL_TRUE);

	// w³¹czenie zapisu sk³adowych RGBA do bufora kolorów
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	// test bufora szablonowego
	glStencilFunc(stencil_func, 0, 1);

	// wy³¹czenie testu bufora g³êbokoœci
	glDisable(GL_DEPTH_TEST);

	// rysowanie wybranej strony wielok¹tów
	glCullFace(cull_face);

	// wyœwietlenie obiektu A
	glCallList(A);

	// wy³¹czenie bufora szablonowego
	glDisable(GL_STENCIL_TEST);
}


void AND(GLint o1, GLint o2)
{
	// elementy obiektu A znajduj¹ce siê we wnêtrzu B
	Inside(o1, o2, GL_BACK, GL_NOTEQUAL);

	// wy³¹czenie zapisu sk³adowych RGBA do bufora kolorów
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	// w³¹czenie testu bufora g³êbokoœci
	glEnable(GL_DEPTH_TEST);

	// wy³¹czenie bufora szablonowego
	glDisable(GL_STENCIL_TEST);

	// wybór funkcji do testu bufora g³êbokoœci
	glDepthFunc(GL_ALWAYS);

	// wyœwietlenie obiektu B
	glCallList(o2);

	// wybór funkcji do testu bufora g³êbokoœci
	glDepthFunc(GL_LESS);

	// elementy obiektu B znajduj¹ce siê we wnêtrzu A
	Inside(o2, o1, GL_BACK, GL_NOTEQUAL);

	glDisable(GL_DEPTH_TEST);
}

void SUB(GLint o1, GLint o2)
{
	// elementy obiektu A znajduj¹ce siê we wnêtrzu B
	Inside(o1, o2, GL_FRONT, GL_NOTEQUAL);

	// wy³¹czenie zapisu sk³adowych RGBA do bufora kolorów
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	// w³¹czenie testu bufora g³êbokoœci
	glEnable(GL_DEPTH_TEST);

	// wy³¹czenie bufora szablonowego
	glDisable(GL_STENCIL_TEST);

	// wybór funkcji do testu bufora g³êbokoœci
	glDepthFunc(GL_ALWAYS);

	// wyœwietlenie obiektu B
	glCallList(o2);

	// wybór funkcji do testu bufora g³êbokoœci
	glDepthFunc(GL_LESS);

	// elementy obiektu B znajduj¹ce siê we wnêtrzu A
	Inside(o2, o1, GL_BACK, GL_EQUAL);

	glDisable(GL_DEPTH_TEST);
}

// funkcja generuj¹ca scenê 3D

void DisplayScene()
{
	// kolor t³a - zawartoœæ bufora koloru
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// czyszczenie bufora koloru, bufora g³êbokoœci i bufora szablonowego
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// wybór macierzy modelowania
	glMatrixMode(GL_MODELVIEW);

	// macierz modelowania = macierz jednostkowa
	glLoadIdentity();

	// przesuniêcie uk³adu wspó³rzêdnych obiektów do œrodka bry³y odcinania
	glTranslatef(0, 0, -(near_ + far_) / 2);

	// obroty ca³ej sceny
	glRotatef(rotatex, 1.0, 0.0, 0.0);
	glRotatef(rotatey, 0.0, 1.0, 0.0);

	// w³¹czenie oœwietlenia
	glEnable(GL_LIGHTING);

	// w³¹czenie œwiat³a GL_LIGHT0
	glEnable(GL_LIGHT0);

	// w³¹czenie automatycznej normalizacji wektorów normalnych
	glEnable(GL_NORMALIZE);

	// w³¹czenie obs³ugi w³aœciwoœci materia³ów
	glEnable(GL_COLOR_MATERIAL);

	// w³aœciwoœci materia³u okreœlone przez kolor wierzcho³ków
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// w³¹czenie rysowania wybranej strony wielok¹tów
	glEnable(GL_CULL_FACE);

	// v1
	if (csg_op == WARIANT1)
	{
		// w³¹czenie testu bufora g³êbokoœci
		glEnable(GL_DEPTH_TEST);

		SUB(Ostroslup, Kula);
		glClear(GL_STENCIL_BUFFER_BIT);
		AND(Torus, Kula);

		// wy³¹czenie testu bufora g³êbokoœci
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

	// skierowanie poleceñ do wykonania
	glFlush();

	// zamiana buforów koloru
	glutSwapBuffers();
}

// zmiana wielkoœci okna

void Reshape(int width, int height)
{
	// obszar renderingu - ca³e okno
	glViewport(0, 0, width, height);

	// wybór macierzy rzutowania
	glMatrixMode(GL_PROJECTION);

	// macierz rzutowania = macierz jednostkowa
	glLoadIdentity();

	// parametry bry³y obcinania
	if (aspect == ASPECT_1_1)
	{
		// wysokoœæ okna wiêksza od wysokoœci okna
		if (width < height && width > 0)
			glFrustum(left, right, bottom * height / width, top * height / width, near_, far_);
		else

			// szerokoœæ okna wiêksza lub równa wysokoœci okna
		if (width >= height && height > 0)
			glFrustum(left * width / height, right * width / height, bottom, top, near_, far_);

	}
	else
		glFrustum(left, right, bottom, top, near_, far_);

	// generowanie sceny 3D
	DisplayScene();
}

// obs³uga przycisków myszki

void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		// zapamiêtanie stanu lewego przycisku myszki
		button_state = state;

		// zapamiêtanie po³o¿enia kursora myszki
		if (state == GLUT_DOWN)
		{
			button_x = x;
			button_y = y;
		}
	}
}

// obs³uga ruchu kursora myszki

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

// obs³uga menu podrêcznego

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

		// obszar renderingu - ca³e okno
	case FULL_WINDOW:
		aspect = FULL_WINDOW;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// obszar renderingu - aspekt 1:1
	case ASPECT_1_1:
		aspect = ASPECT_1_1;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// wyjœcie
	case EXIT:
		exit(0);
	}
}

// utworzenie list wyœwietlania

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

	// rozmiary g³ównego okna programu
	glutInitWindowSize(500, 500);

	// utworzenie g³ównego okna programu
	glutCreateWindow("CSG");

	// do³¹czenie funkcji generuj¹cej scenê 3D
	glutDisplayFunc(DisplayScene);

	// do³¹czenie funkcji wywo³ywanej przy zmianie rozmiaru okna
	glutReshapeFunc(Reshape);

	// obs³uga przycisków myszki
	glutMouseFunc(MouseButton);

	// obs³uga ruchu kursora myszki
	glutMotionFunc(MouseMotion);

	// utworzenie podmenu - Operacja CSG
	int MenuCSGOp = glutCreateMenu(Menu);
	glutAddMenuEntry("Wariant 1", WARIANT1);
	glutAddMenuEntry("Wariant 2", WARIANT2);
	glutAddMenuEntry("Wariant 3", WARIANT3);

	// utworzenie podmenu - Aspekt obrazu
	int MenuAspect = glutCreateMenu(Menu);
	glutAddMenuEntry("Aspekt obrazu - ca³e okno", FULL_WINDOW);
	glutAddMenuEntry("Aspekt obrazu 1:1", ASPECT_1_1);

	// menu g³ówne
	glutCreateMenu(Menu);
	glutAddSubMenu("Operacja CSG", MenuCSGOp);

	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyjœcie", EXIT);


	// okreœlenie przycisku myszki obs³uguj¹cego menu podrêczne
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// utworzenie list wyœwietlania
	GenerateDisplayLists();

	// wprowadzenie programu do obs³ugi pêtli komunikatów
	glutMainLoop();
	return 0;
}