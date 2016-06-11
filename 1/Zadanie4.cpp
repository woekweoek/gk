// Zadanie4.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

GLfloat angle = 0;

///////////////////////////////////////////////////////////
// Wywo�ywana w celu przerysowania sceny
void RenderScene(void)
{
	// Wyczyszczenie okna aktualnym kolorem czyszczcym ��
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();
	// Obr�t wok� osi Y o aktualny k�t
	glRotatef(angle, 0.0f, 1.0f, 0.0f);

	// Aktualny kolor rysujacy - ��ty
	// ����������R ����G ����B �
	glColor3f(1.0f, 1.0f, 0.0f);

	glBegin(GL_POLYGON);
	glVertex3f(0.0f, 25.0f, 0.0f);
	glVertex3f(-25.0f, -25.0f, 0.0f);
	glVertex3f(25.0f, -25.0f, 0.0f);
	glEnd();

	// Zamiana bufor�w
	glutSwapBuffers();
}
///////////////////////////////////////////////////////////
// Konfiguracja stanu renderowania �
void SetupRC(void)
{
	// Ustalenie br�zowego koloru czyszczcego ����
	glClearColor(0.6f, 0.4f, 0.12f, 1.0f);
}
///////////////////////////////////////////////////////////
// Wywo�ywana przez bibliotek GLUT w przypadku zmiany rozmiaru okna
void ChangeSize(int w, int h)
{
	GLfloat aspectRatio;

	// Zabezpieczenie przed dzieleniem przez zero �
	if (h == 0) h = 1;
	// Ustawienie wielko�ci widoku na r�wn� wielko�ci okna ����
	glViewport(0, 0, w, h);
	// Ustalenie uk�adu wsp�rz�dnych �
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Wyznaczenie przestrzeni ograniczaj�cej (lewy, prawy, dolny, g�rny, bliski, odleg�y) �
	aspectRatio = (GLfloat)w / (GLfloat)h;
	if (w <= h)
		glOrtho(-100.0, 100.0, -100 / aspectRatio, 100.0 / aspectRatio, 1000.0, -1000.0);
	else
		glOrtho(-100.0 * aspectRatio, 100.0 * aspectRatio, -100.0, 100.0, 1000.0, -1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Rotation(void)
{
	// Zwi�kszenie k�ta o 8 stopni 
	if (angle == 360) angle = 0;
	angle += 8;
	// Wymuszenie ponownego przerysowania okna
	glutPostRedisplay();
}

///////////////////////////////////////////////////////////
// G��wny punkt wejcia programu
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Zaadanie 4");
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	glutIdleFunc(Rotation);
	SetupRC();
	glutMainLoop();
	return 0;
}