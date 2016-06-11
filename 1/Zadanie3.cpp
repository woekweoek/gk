// Zadanie3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Pocztkowy rozmiar i pozycja tr�jk�ta
GLfloat x1 = 75.0f;
GLfloat x2 = 50.0f;
GLfloat x3 = 100.0f;
GLfloat y1 = 100.0f;
GLfloat y2 = 50.0f;
GLfloat y3 = 50.0f;

// Rozmiar kroku (liczba pikseli) w osi x i y
GLfloat xstep = 1.0f;
GLfloat ystep = 1.0f;
// Dane zmieniajcych si� rozmiar�w okna
GLfloat windowWidth;
GLfloat windowHeight;
///////////////////////////////////////////////////////////
// Wywo�ywana w celu przerysowania sceny
void RenderScene(void) 
{
	// Wyczyszczenie okna aktualnym kolorem czyszcz�cym
	glClear(GL_COLOR_BUFFER_BIT);

	// Aktualny kolor rysujacy - ��ty
	// ����������R ����G ����B �
	glColor3f(1.0f, 1.0f, 0.0f);

	// Rysowanie tr�jk�ta
	glBegin(GL_POLYGON);
	glVertex3f(x1, y1, 0.0f); // G�rny wierzcho�ek
	glVertex3f(x2, y2, 0.0f); // Lewy wierzcho�ek
	glVertex3f(x3, y3, 0.0f); // Prawy wierzcho�ek
	glEnd();

	// Wys�anie polece� do wykonania - !!! dla animacji to jest inne polecenie
	glutSwapBuffers();
}
///////////////////////////////////////////////////////////
// Wywo�ywana przez bibliotek GLUT w czasie, gdy okno nie
// jest przesuwane ani nie jest zmieniana jego wielko��
void TimerFunction(int value) 
{
	// Odwr�cenie kierunku, je�eli osi�gni�to lew� lub praw� kraw�d�
	if (x3 > windowWidth || x2 < 0)
		xstep = -xstep;

	// Odwr�cenie kierunku, je�eli osi�gni�to doln� lub g�rn� kraw�d�
	if (y1 >= windowHeight || ((y2 < 0) && (y3 < 0)))
		ystep = -ystep;

	// Kontrola obramowania. Wykonywana jest na wypadek, gdyby okno ����
	// zmniejszy�o swoj wielko�� w czasie, gdy tr�jk�t odbija� si� od ����
	// kraw�dzi, co mog�oby spowodowa�, �e znalaz� by si� poza �����
	// przestrzeni� ograniczajc�. ����
	if (x1 > windowWidth )
		x1 = windowWidth - 1;

	if (y1 > windowHeight )
		y1 = windowHeight - 1;

	// Wykonanie przesuni�cia tr�jk�ta
	x1 += xstep;
	x2 += xstep;
	x3 += xstep;
	y1 += ystep;
	y2 += ystep;
	y3 += ystep;

	// Ponowne rysowanie sceny z nowymi wsp�rz�dnymi �
	glutPostRedisplay();
	glutTimerFunc(33, TimerFunction, 1);
}
///////////////////////////////////////////////////////////
// Konfigurowanie stanu renderowania
void SetupRC(void) 
{
	// Ustalenie br�zowego koloru czyszczcego ����
	glClearColor(0.6f, 0.4f, 0.12f, 1.0f);
}
///////////////////////////////////////////////////////////
// Wywo�ywana przez bibliotek GLUT przy ka�dej zmianie wielko�ci okna
void ChangeSize(GLsizei w, GLsizei h)
{
	// Zabezpieczenie przed dzieleniem przez zero
	if (h == 0)
		h = 1;

	// Ustalenie wielko�ci widoku zgodnego z rozmiarami okna
	glViewport(0, 0, w, h);

	// Ustalenie uk�adu wsp�rz�dnych
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w <= h)
	{
		windowHeight = 250.0f*h / w;
		windowWidth = 250.0f;
	}
	else
	{
		windowWidth = 250.0f*w / h;
		windowHeight = 250.0f;
	}

	// Ustanowienie przestrzeni ograniczaj�cej (lewo, prawo, d�, g�ra, blisko, daleko)
	glOrtho(0.0f, windowWidth, 0.0f, windowHeight, 1.0f, -1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
///////////////////////////////////////////////////////////
// G��wny punkt wej�cia programu
void main(int argc, char* argv[]) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Zadanie 3");
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	glutTimerFunc(33, TimerFunction, 1);
	SetupRC();
	glutMainLoop();
}