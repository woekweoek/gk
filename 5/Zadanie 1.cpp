// Zadanie 1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <GL/glut.h>
#include <GL/glext.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "materials.h"
#include "colors.h"

// wskaźnik dostępności rozszerzenia EXT_rescale_normal

bool rescale_normal = false;

// wskaźnik na funkcję glWindowPos2i

PFNGLWINDOWPOS2IPROC glWindowPos2i = NULL;

// stałe do obsługi menu podręcznego

enum
{
	//Tryb
	SK,  // światło kierunkowe
	SO,  // światło otaczające
	RE,  // reflektor
	WN,  // wektory normalne

	BRASS, // mosiądz
	BRONZE, // brąz
	POLISHED_BRONZE, // polerowany brąz
	CHROME, // chrom
	COPPER, // miedź
	POLISHED_COPPER, // polerowana miedź
	GOLD, // złoto
	POLISHED_GOLD, // polerowane złoto
	PEWTER, // grafit (cyna z ołowiem)
	SILVER,  // srebro
	POLISHED_SILVER,  // polerowane srebro
	EMERALD, // szmaragd
	JADE,  // jadeit
	OBSIDIAN,  // obsydian
	PEARL,  // perła
	RUBY,  // rubin
	TURQUOISE,  // turkus
	BLACK_PLASTIC,  // czarny plastik
	BLACK_RUBBER,  // czarna guma
	SWIATLO_KIERUNKOWE, // swiatlo kierunowe
	SWIATLO_OTACZAJACE, // swiatlo otaczajace
	REFLEKTOR, // reflektor
	NORMALS_SMOOTH,  // jeden wektor normalny na wierzchołek
	NORMALS_FLAT, // jeden wektor normalny na ścianę
	FULL_WINDOW,  // aspekt obrazu - całe okno
	ASPECT_1_1,  // aspekt obrazu 1:1
	EXIT  // wyjście
};

// aspekt obrazu

int aspect = FULL_WINDOW;

//  Aktualny tryb wyświetlania

int activity = WN;

// usunięcie definicji makr near i far

#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif

// rozmiary bryły obcinania

const GLdouble left = -2.0;
const GLdouble right = 2.0;
const GLdouble bottom = -2.0;
const GLdouble top = 2.0;
const GLdouble near = 3.0;
const GLdouble far = 7.0;

// kąty obrotu

GLfloat rotatex = 0.0;
GLfloat rotatey = 0.0;

// wskaźnik naciśnięcia lewego przycisku myszki

int button_state = GLUT_UP;

// poło¿enie kursora myszki

int button_x, button_y;

// współczynnik skalowania

GLfloat scale = 1.0;

// właściwości materiału - domyślnie mosiądz

const GLfloat *ambient = BrassAmbient;
const GLfloat *diffuse = BrassDiffuse;
const GLfloat *specular = BrassSpecular;
GLfloat shininess = BrassShininess;

// kierunek źródła światła

GLfloat light_position[4] =
{
	0.0, 0.0, 2.0, 0.0
};

// składowe globalnego światła otaczającego
// wartośæ początkowa odpowiada wartości domyślnej
// składowych tego światła

GLfloat ambient_light[4] =
{
	0.2, 0.2, 0.2, 1.0
};

// kąty obrotu kierunku źródła światła

GLfloat light_rotatex = 0.0;
GLfloat light_rotatey = 0.0;

// wektory normalne

int normals = NORMALS_FLAT;

// współrzędne wierzchołków piramidy o trójkątnej podstawie

GLfloat vertex[4 * 3] =
{
	0.000, 0.800, 0.000,  // v0
	0.000, 0.000, -0.500,  // v1
	0.500, 0.000, 0.500,  // v2
	-0.500, 0.000, 0.500,  // v3
};

// opis ścian piramidy

int triangles[4 * 3] =
{
	1, 2, 3,
	0, 3, 2,
	0, 1, 3,
	0, 2, 1,
};

// kąty obrotu położenia źródła światła - reflektor

GLfloat light_rotatexx = 0.0;
GLfloat light_rotateyy = 0.0;

// kierunek reflektora

GLfloat spot_direction[3] =
{
	0.0, 0.0, -1.0
};

// kąt odciêcia reflektora

GLfloat spot_cutoff = 180.0;

// wykładnik tłumienia kątowego reflektora

GLfloat spot_exponent = 128.0;

// stały współczynnik tłumienia światła

GLfloat constant_attenuation = 1.0;

// liniowy współczynnik tłumienia światła

GLfloat linear_attenuation = 0.0;

// kwadratowy współczynnik tłumienia światła

GLfloat quadratic_attenuation = 0.0;

void DrawString(GLint xx, GLint yy, char *string)
{

	// położenie napisu
	glRasterPos2i(xx, yy);

	// wyświetlenie napisu
	int len = strlen(string);
	for (int i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, string[i]);
}


// obliczanie wektora normalnego dla wybranej ściany

void Normal(GLfloat *n, int i)
{
	GLfloat v1[3], v2[3];

	// obliczenie wektorów na podstawie współrzędnych wierzchołków trójkątów
	v1[0] = vertex[3 * triangles[3 * i + 1] + 0] - vertex[3 * triangles[3 * i + 0] + 0];
	v1[1] = vertex[3 * triangles[3 * i + 1] + 1] - vertex[3 * triangles[3 * i + 0] + 1];
	v1[2] = vertex[3 * triangles[3 * i + 1] + 2] - vertex[3 * triangles[3 * i + 0] + 2];
	v2[0] = vertex[3 * triangles[3 * i + 2] + 0] - vertex[3 * triangles[3 * i + 1] + 0];
	v2[1] = vertex[3 * triangles[3 * i + 2] + 1] - vertex[3 * triangles[3 * i + 1] + 1];
	v2[2] = vertex[3 * triangles[3 * i + 2] + 2] - vertex[3 * triangles[3 * i + 1] + 2];

	// obliczenie waktora normalnego przy pomocy iloczynu wektorowego
	n[0] = v1[1] * v2[2] - v1[2] * v2[1];
	n[1] = v1[2] * v2[0] - v1[0] * v2[2];
	n[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

// normalizacja wektora

void Normalize(GLfloat *v)
{
	// obliczenie długości wektora
	GLfloat d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

	// normalizacja wektora
	if (d)
	{
		v[0] /= d;
		v[1] /= d;
		v[2] /= d;
	}
}

void Reflektor_Display()
{
	// kolor tła - zawartośæ bufora koloru
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// czyszczenie bufora koloru i bufora głêbokości
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// wybór macierzy modelowania
	glMatrixMode(GL_MODELVIEW);

	// macierz modelowania = macierz jednostkowa
	glLoadIdentity();

	// włączenie testu bufora głêbokości
	glEnable(GL_DEPTH_TEST);

	// włączenie oświetlenia
	glEnable(GL_LIGHTING);

	// włączenie światła GL_LIGHT0
	glEnable(GL_LIGHT0);

	// włączenie automatycznej normalizacji wektorów normalnych
	glEnable(GL_NORMALIZE);

	// przesuniêcie układu współrzêdnych obiektu do środka bryły odcinania
	glTranslatef(0, 0, -(near + far) / 2);

	// obroty obiektu
	glRotatef(rotatex, 1.0, 0, 0);
	glRotatef(rotatey, 0, 1.0, 0);

	// skalowanie obiektu - klawisze "+" i "-"
	glScalef(scale, scale, scale);

	// właściwości materiału
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);

	// kąt odciêcia reflektora
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spot_cutoff);

	// wykładnik tłumienia kątowego reflektora
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, spot_exponent);

	// stały współczynnik tłumienia światła,
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, constant_attenuation);

	// liniowy współczynnik tłumienia światła
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, linear_attenuation);

	// kwadratowy współczynnik tłumienia światła
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, quadratic_attenuation);

	// zmiana położenia źródła światła jest wykonywana niezależnie
	// od obrotów obiektu, stąd odłożenie na stos macierzy modelowania
	glPushMatrix();

	// macierz modelowania = macierz jednostkowa
	glLoadIdentity();

	// przesuniêcie układu współrzêdnych źródła światła do środka bryły odcinania
	glTranslatef(0, 0, -(near + far) / 2);

	// obroty położenia źródła światła - klawisze kursora
	glRotatef(light_rotatexx, 1.0, 0, 0);
	glRotatef(light_rotateyy, 0, 1.0, 0);

	// przesuniêcie źródła światła
	glTranslatef(light_position[0], light_position[1], light_position[2]);

	// ustalenie pozycji źródła światła
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	// ustalenie kierunku reflektora
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);

	// odłożenie na stos zmiennych stanu związanych z oświetleniem sceny
	glPushAttrib(GL_LIGHTING_BIT);

	// wyłacznie źródła światła GL_LIGHT0
	glDisable(GL_LIGHT0);

	// materiał imitujący świecenie kuli na czerwono
	glMaterialfv(GL_FRONT, GL_EMISSION, Red);

	// narysowanie kuli położonej w początku źródła światła
	glutSolidSphere(0.1, 30, 20);

	// przywrócenie zmiennych stanu związanych z oświetleniem sceny
	glPopAttrib();

	// przywrócenie pierwotnej macierzy modelowania
	glPopMatrix();

	glBegin(GL_TRIANGLES);
	glVertex3f(0.0, 0.0, -0.5);
	glVertex3f(0.5, 0.0, 0.5);
	glVertex3f(-0.5, 0.0, 0.5);

	glVertex3f(0.0, 0.8, 0.0);
	glVertex3f(-0.5, 0.0, 0.5);
	glVertex3f(0.5, 0.0, 0.5);

	glVertex3f(0.0, 0.8, 0.0);
	glVertex3f(0.0, 0.0, -0.5);
	glVertex3f(-0.5, 0.0, 0.5);

	glVertex3f(0.0, 0.8, 0.0);
	glVertex3f(0.0, 0.0, 0.5);
	glVertex3f(0.0, 0.0, -0.5);

	glEnd();

	// informacje o wartościach modyfikowanych
	// parametrów źródła światała GL_LIGHT0
	char string[200];
	GLfloat vec[4];
	glColor3fv(Black);

	// trzeba odpowiednio przekształciæ układ współrzędnych
	// aby napis znajdował się na samej "górze" bryły obcinania
	glLoadIdentity();
	glTranslatef(0, 0, -near);


	// kierunek źródła światła
	glGetLightfv(GL_LIGHT0, GL_POSITION, vec);
	sprintf(string, "GL_POSITION = (%f,%f,%f,%f)", vec[0], vec[1], vec[2], vec[3]);
	DrawString((float)left, (float)bottom , string);

	glGetLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, vec);
	sprintf(string, "GL_SPOT_DIRECTION = (%f,%f,%f)", vec[0], vec[1], vec[2]);

	// kąt odciêcia reflektora
	glGetLightfv(GL_LIGHT0, GL_SPOT_CUTOFF, vec);
	sprintf(string, "GL_SPOT_CUTOFF = %f", vec[0]);
	DrawString(2, 30, string);

	// wykładnik tłumienia kątowego reflektora
	glGetLightfv(GL_LIGHT0, GL_SPOT_EXPONENT, vec);
	sprintf(string, "GL_SPOT_EXPONENT = %f", vec[0]);
	DrawString(2, 44, string);

	// stały współczynnik tłumienia światła,
	glGetLightfv(GL_LIGHT0, GL_CONSTANT_ATTENUATION, vec);
	sprintf(string, "GL_CONSTANT_ATTENUATION = %f", vec[0]);
	DrawString(2, 58, string);

	// liniowy współczynnik tłumienia światła
	glGetLightfv(GL_LIGHT0, GL_LINEAR_ATTENUATION, vec);
	sprintf(string, "GL_LINEAR_ATTENUATION = %f", vec[0]);
	DrawString(2, 72, string);

	// kwadratowy współczynnik tłumienia światła
	glGetLightfv(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, vec);
	sprintf(string, "GL_QUADRATIC_ATTENUATION = %f", vec[0]);
	DrawString(2, 86, string);

	// skierowanie poleceñ do wykonania
	glFlush();

	// zamiana buforów koloru
	glutSwapBuffers();
}

void SO_Display()
{
	// kolor tła - zawartośæ bufora koloru
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// czyszczenie bufora koloru i bufora głębokości
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// wybór macierzy modelowania
	glMatrixMode(GL_MODELVIEW);

	// macierz modelowania = macierz jednostkowa
	glLoadIdentity();

	// przesunięcie układu współrzędnych sześcianu do środka bryły odcinania
	glTranslatef(0, 0, -(near + far) / 2);

	// obroty sześcianu
	glRotatef(rotatex, 1.0, 0, 0);
	glRotatef(rotatey, 0, 1.0, 0);

	// włączenie oświetlenia
	glEnable(GL_LIGHTING);

	// parametry globalnego światła otaczającego
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light);

	// włączenie obsługi właściwości materiałów
	glEnable(GL_COLOR_MATERIAL);

	// właściwości materiału określone przez kolor wierzchołków
	glColorMaterial(GL_FRONT, GL_AMBIENT);

	// włączenie testu bufora głębokości
	glEnable(GL_DEPTH_TEST);

	glBegin(GL_TRIANGLES);
	glColor3fv(Red);
	glVertex3f(0.0, 0.0, -0.5);
	glVertex3f(0.5, 0.0, 0.5);
	glVertex3f(-0.5, 0.0, 0.5);

	glColor3fv(Cyan);
	glVertex3f(0.0, 0.8, 0.0);
	glVertex3f(-0.5, 0.0, 0.5);
	glVertex3f(0.5, 0.0, 0.5);
	
	glColor3fv(Yellow);
	glVertex3f(0.0, 0.8, 0.0);
	glVertex3f(0.0, 0.0, -0.5);
	glVertex3f(-0.5, 0.0, 0.5);

	glColor3fv(Blue);
	glVertex3f(0.0, 0.8, 0.0);
	glVertex3f(0.5, 0.0, 0.5);
	glVertex3f(0.0, 0.0, -0.5);

	glEnd();


	// wyłączenie oświetlenia
	glDisable(GL_LIGHTING);

	// wyłączenie obsługi właściwości materiałów
	glDisable(GL_COLOR_MATERIAL);

	// wyświetlenie składowych globalnego światła otaczającego
	char string[100];
	GLfloat rgba[4];
	glColor3fv(Black);

	// pobranie wartości składowych światła otaczającego
	// (oczywiście wartości te odpowiadają tablicy ambient_light)
	glGetFloatv(GL_LIGHT_MODEL_AMBIENT, rgba);
	sprintf(string, "AMBIENT: R=%f G=%f B=%f A=%f", rgba[0], rgba[1], rgba[2], rgba[3]);

	// trzeba odpowiednio przekształciæ układ współrzędnych
	// aby napis znajdował się na samej "górze" bryły obcinania
	glLoadIdentity();
	glTranslatef(0, 0, -near);

	// narysowanie napisu
	DrawString((float)left, (float)bottom, string); 


	// skierowanie poleceñ do wykonania
	glFlush();

	// zamiana buforów koloru
	glutSwapBuffers();
}


void SK_Display()
{
// kolor tła - zawartośæ bufora koloru
glClearColor(1.0, 1.0, 1.0, 1.0);

// czyszczenie bufora koloru i bufora głębokości
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// wybór macierzy modelowania
glMatrixMode(GL_MODELVIEW);

// macierz modelowania = macierz jednostkowa
glLoadIdentity();

// włączenie testu bufora głębokości
glEnable(GL_DEPTH_TEST);

// włączenie oświetlenia
glEnable(GL_LIGHTING);

// włączenie światła GL_LIGHT0
glEnable(GL_LIGHT0);

// włączenie automatycznej normalizacji wektorów normalnych
glEnable(GL_NORMALIZE);

// przesunięcie układu współrzędnych obiektu do środka bryły odcinania
glTranslatef(0, 0, -(near + far) / 2);

// obroty obiektu
glRotatef(rotatex, 1.0, 0, 0);
glRotatef(rotatey, 0, 1.0, 0);

// skalowanie obiektu - klawisze "+" i "-"
glScalef(scale, scale, scale);

// właściwości materiału
glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
glMaterialf(GL_FRONT, GL_SHININESS, shininess);

// zmiana kierunku źródła światła jest wykonywana niezależnie
// od obrotów obiektu, stąd odłożenie na stos macierzy modelowania
glPushMatrix();

// macierz modelowania = macierz jednostkowa
glLoadIdentity();

// obroty kierunku źródła światła - klawisze kursora
glRotatef(light_rotatex, 1.0, 0, 0);
glRotatef(light_rotatey, 0, 1.0, 0);

// ustalenie kierunku źródła światła
glLightfv(GL_LIGHT0, GL_POSITION, light_position);

// przywrócenie pierwotnej macierzy modelowania
glPopMatrix();

glBegin(GL_TRIANGLES);
glVertex3f(0.0, 0.0, -0.5);
glVertex3f(0.5, 0.0, 0.5);
glVertex3f(-0.5, 0.0, 0.5);

glVertex3f(0.0, 0.8, 0.0);
glVertex3f(-0.5, 0.0, 0.5);
glVertex3f(0.5, 0.0, 0.5);

glVertex3f(0.0, 0.8, 0.0);
glVertex3f(0.0, 0.0, -0.5);
glVertex3f(-0.5, 0.0, 0.5);

glVertex3f(0.0, 0.8, 0.0);
glVertex3f(0.0, 0.0, 0.5);
glVertex3f(0.0, 0.0, -0.5);

glEnd();

// skierowanie poleceń do wykonania
glFlush();

// zamiana buforów koloru
glutSwapBuffers();
}

void NormalizeDisplay()
{
	// kolor tła - zawartość bufora koloru
	glClearColor(1.0, 1.0, 1.0, 1.0);

	// czyszczenie bufora koloru i bufora głębokości
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// wybór macierzy modelowania
	glMatrixMode(GL_MODELVIEW);

	// macierz modelowania = macierz jednostkowa
	glLoadIdentity();

	// przesunięcie układu współrzędnych obiektu do środka bryły odcinania
	glTranslatef(0, 0, -(near + far) / 2);

	// obroty obiektu
	glRotatef(rotatex, 1.0, 0, 0);
	glRotatef(rotatey, 0, 1.0, 0);

	// skalowanie obiektu - klawisze "+" i "-"
	glScalef(scale, scale, scale);

	// włączenie testu bufora głębokości
	glEnable(GL_DEPTH_TEST);

	// włączenie oświetlenia
	glEnable(GL_LIGHTING);

	// włączenie światła GL_LIGHT0 z parametrami domyślnymi
	glEnable(GL_LIGHT0);

	// właściwości materiału
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);

	// włączenie automatycznej normalizacji wektorów normalnych
	// lub automatycznego skalowania jednostkowych wektorów normalnych
	if (rescale_normal == true)
		glEnable(GL_RESCALE_NORMAL);
	else
		glEnable(GL_NORMALIZE);

	// początek definicji obiektu
	glBegin(GL_TRIANGLES);

	// generowanie obiektu gładkiego - jeden uśredniony
	// wektor normalny na wierzchołek
	if (normals == NORMALS_SMOOTH)
	for (int i = 0; i < 4; i++)
	{
		// obliczanie wektora normalnego dla pierwszego wierzchołka
		GLfloat n[3];
		n[0] = n[1] = n[2] = 0.0;

		// wyszukanie wszystkich ścian posiadających bie¿ący wierzchołek
		for (int j = 0; j < 4; j++)
		if (3 * triangles[3 * i + 0] == 3 * triangles[3 * j + 0] ||
			3 * triangles[3 * i + 0] == 3 * triangles[3 * j + 1] ||
			3 * triangles[3 * i + 0] == 3 * triangles[3 * j + 2])
		{
			// dodawanie wektorów normalnych poszczególnych ścian
			GLfloat nv[3];
			Normal(nv, j);
			n[0] += nv[0];
			n[1] += nv[1];
			n[2] += nv[2];
		}

		// uśredniony wektor normalny jest normalizowany tylko, gdy biblioteka
		// obsługuje automatyczne skalowania jednostkowych wektorów normalnych
		if (rescale_normal == true)
			Normalize(n);
		glNormal3fv(n);
		glVertex3fv(&vertex[3 * triangles[3 * i + 0]]);

		// obliczanie wektora normalnego dla drugiego wierzchołka
		n[0] = n[1] = n[2] = 0.0;

		// wyszukanie wszystkich ścian posiadających bie¿ący wierzchołek
		for (int j = 0; j < 4; j++)
		if (3 * triangles[3 * i + 1] == 3 * triangles[3 * j + 0] ||
			3 * triangles[3 * i + 1] == 3 * triangles[3 * j + 1] ||
			3 * triangles[3 * i + 1] == 3 * triangles[3 * j + 2])
		{
			// dodawanie wektorów normalnych poszczególnych ścian
			GLfloat nv[3];
			Normal(nv, j);
			n[0] += nv[0];
			n[1] += nv[1];
			n[2] += nv[2];
		}

		// uśredniony wektor normalny jest normalizowany tylko, gdy biblioteka
		// obsługuje automatyczne skalowania jednostkowych wektorów normalnych
		if (rescale_normal == true)
			Normalize(n);
		glNormal3fv(n);
		glVertex3fv(&vertex[3 * triangles[3 * i + 1]]);

		// obliczanie wektora normalnego dla trzeciego wierzchołka
		n[0] = n[1] = n[2] = 0.0;

		// wyszukanie wszystkich ścian posiadających bie¿ący wierzchołek
		for (int j = 0; j < 4; j++)
		if (3 * triangles[3 * i + 2] == 3 * triangles[3 * j + 0] ||
			3 * triangles[3 * i + 2] == 3 * triangles[3 * j + 1] ||
			3 * triangles[3 * i + 2] == 3 * triangles[3 * j + 2])
		{
			// dodawanie wektorów normalnych poszczególnych ścian
			GLfloat nv[3];
			Normal(nv, j);
			n[0] += nv[0];
			n[1] += nv[1];
			n[2] += nv[2];
		}

		// uśredniony wektor normalny jest normalizowany tylko, gdy biblioteka
		// obsługuje automatyczne skalowania jednostkowych wektorów normalnych
		if (rescale_normal == true)
			Normalize(n);
		glNormal3fv(n);
		glVertex3fv(&vertex[3 * triangles[3 * i + 2]]);
	}
	else

		// generowanie obiektu "płaskiego" - jeden wektor normalny na ścianę
	for (int i = 0; i < 4; i++)
	{
		GLfloat n[3];
		Normal(n, i);

		// uśredniony wektor normalny jest normalizowany tylko, gdy biblioteka
		// obsługuje automatyczne skalowania jednostkowych wektorów normalnych
		if (rescale_normal == true)
			Normalize(n);
		glNormal3fv(n);
		glVertex3fv(&vertex[3 * triangles[3 * i + 0]]);
		glVertex3fv(&vertex[3 * triangles[3 * i + 1]]);
		glVertex3fv(&vertex[3 * triangles[3 * i + 2]]);
	}

	// koniec definicji obiektu
	glEnd();

	// skierowanie poleceñ do wykonania
	glFlush();

	// zamiana buforów koloru
	glutSwapBuffers();
}

// funkcja generująca scenę 3D

void Display()
{
	switch (activity)
	{

	case WN:
		NormalizeDisplay();
		break;

	case SK:
		SK_Display();
		break;

	case SO:
		SO_Display();
		break;

	case RE:
		Reflektor_Display();
		break;
	}
}


// zmiana wielkości okna

void Reshape(int width, int height)
{
	// obszar renderingu - całe okno
	glViewport(0, 0, width, height);

	// wybór macierzy rzutowania
	glMatrixMode(GL_PROJECTION);

	// macierz rzutowania = macierz jednostkowa
	glLoadIdentity();

	// parametry bryły obcinania
	if (aspect == ASPECT_1_1)
	{
		// wysokość okna większa od wysokości okna
		if (width < height && width > 0)
			glFrustum(left, right, bottom*height / width, top*height / width, near, far);
		else

			// szerokość okna większa lub równa wysokości okna
		if (width >= height && height > 0)
			glFrustum(left*width / height, right*width / height, bottom, top, near, far);
	}
	else
		glFrustum(left, right, bottom, top, near, far);

	// generowanie sceny 3D
	Display();
}

// obsługa klawiatury

void Keyboard(unsigned char key, int x, int y)
{
	// klawisz +
	if (key == '+')
		scale += 0.05;
	else

		// klawisz -
	if (key == '-' && scale > 0.05)
		scale -= 0.05;

	if (activity == SO)
	{

		// zmiana wartości składowej R
		if (key == 'R' && ambient_light[0] < 1.0)
			ambient_light[0] += 0.05;
		else
		if (key == 'r' && ambient_light[0] > -1.0)
			ambient_light[0] -= 0.05;

		// zmiana wartości składowej G
		if (key == 'G' && ambient_light[1] < 1.0)
			ambient_light[1] += 0.05;
		else
		if (key == 'g' && ambient_light[1] > -1.0)
			ambient_light[1] -= 0.05;

		// zmiana wartości składowej B
		if (key == 'B' && ambient_light[2] < 1.0)
			ambient_light[2] += 0.05;
		else
		if (key == 'b' && ambient_light[2] > -1.0)
			ambient_light[2] -= 0.05;
	}
	// narysowanie sceny
	Display();
}

// obsługa klawiszy funkcyjnych i klawiszy kursora

void SpecialKeys(int key, int x, int y)
{
	if (activity == RE)
	{
		switch (key)
		{
			// kursor w lewo
		case GLUT_KEY_LEFT:
			light_rotateyy -= 5;
			break;

			// kursor w prawo
		case GLUT_KEY_RIGHT:
			light_rotateyy += 5;
			break;

			// kursor w dół
		case GLUT_KEY_DOWN:
			light_rotatexx += 5;
			break;

			// kursor w górê
		case GLUT_KEY_UP:
			light_rotatexx -= 5;
			break;
		}
	}
	else
	{

		switch (key)
		{
			// kursor w lewo
		case GLUT_KEY_LEFT:
			rotatey -= 5;
			break;

			// kursor w górę
		case GLUT_KEY_UP:
			rotatex -= 5;
			break;

			// kursor w prawo
		case GLUT_KEY_RIGHT:
			rotatey += 5;
			break;

			// kursor w dół
		case GLUT_KEY_DOWN:
			rotatex += 5;
			break;
		}
	}

	// odrysowanie okna
	Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

// obsługa przycisków myszki

void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		// zapamiętanie stanu lewego przycisku myszki
		button_state = state;

		// zapamiętanie poło¿enia kursora myszki
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
		rotatey += 30 * (right - left) / glutGet(GLUT_WINDOW_WIDTH) * (x - button_x);
		button_x = x;
		rotatex -= 30 * (top - bottom) / glutGet(GLUT_WINDOW_HEIGHT) * (button_y - y);
		button_y = y;
		glutPostRedisplay();
	}
}

// obsługa menu podręcznego

void Menu(int value)
{
	switch (value)
	{
		// materiał - mosiądz
	case BRASS:
		ambient = BrassAmbient;
		diffuse = BrassDiffuse;
		specular = BrassSpecular;
		shininess = BrassShininess;
		Display();
		break;

		// materiał - brąz
	case BRONZE:
		ambient = BronzeAmbient;
		diffuse = BronzeDiffuse;
		specular = BronzeSpecular;
		shininess = BronzeShininess;
		Display();
		break;

		// materiał - polerowany brąz
	case POLISHED_BRONZE:
		ambient = PolishedBronzeAmbient;
		diffuse = PolishedBronzeDiffuse;
		specular = PolishedBronzeSpecular;
		shininess = PolishedBronzeShininess;
		Display();
		break;

		// materiał - chrom
	case CHROME:
		ambient = ChromeAmbient;
		diffuse = ChromeDiffuse;
		specular = ChromeSpecular;
		shininess = ChromeShininess;
		Display();
		break;

		// materiał - miedź
	case COPPER:
		ambient = CopperAmbient;
		diffuse = CopperDiffuse;
		specular = CopperSpecular;
		shininess = CopperShininess;
		Display();
		break;

		// materiał - polerowana miedź
	case POLISHED_COPPER:
		ambient = PolishedCopperAmbient;
		diffuse = PolishedCopperDiffuse;
		specular = PolishedCopperSpecular;
		shininess = PolishedCopperShininess;
		Display();
		break;

		// materiał - złoto
	case GOLD:
		ambient = GoldAmbient;
		diffuse = GoldDiffuse;
		specular = GoldSpecular;
		shininess = GoldShininess;
		Display();
		break;

		// materiał - polerowane złoto
	case POLISHED_GOLD:
		ambient = PolishedGoldAmbient;
		diffuse = PolishedGoldDiffuse;
		specular = PolishedGoldSpecular;
		shininess = PolishedGoldShininess;
		Display();
		break;

		// materiał - grafit (cyna z ołowiem)
	case PEWTER:
		ambient = PewterAmbient;
		diffuse = PewterDiffuse;
		specular = PewterSpecular;
		shininess = PewterShininess;
		Display();
		break;

		// materiał - srebro
	case SILVER:
		ambient = SilverAmbient;
		diffuse = SilverDiffuse;
		specular = SilverSpecular;
		shininess = SilverShininess;
		Display();
		break;

		// materiał - polerowane srebro
	case POLISHED_SILVER:
		ambient = PolishedSilverAmbient;
		diffuse = PolishedSilverDiffuse;
		specular = PolishedSilverSpecular;
		shininess = PolishedSilverShininess;
		Display();
		break;

		// materiał - szmaragd
	case EMERALD:
		ambient = EmeraldAmbient;
		diffuse = EmeraldDiffuse;
		specular = EmeraldSpecular;
		shininess = EmeraldShininess;
		Display();
		break;

		// materiał - jadeit
	case JADE:
		ambient = JadeAmbient;
		diffuse = JadeDiffuse;
		specular = JadeSpecular;
		shininess = JadeShininess;
		Display();
		break;

		// materiał - obsydian
	case OBSIDIAN:
		ambient = ObsidianAmbient;
		diffuse = ObsidianDiffuse;
		specular = ObsidianSpecular;
		shininess = ObsidianShininess;
		Display();
		break;

		// materiał - perła
	case PEARL:
		ambient = PearlAmbient;
		diffuse = PearlDiffuse;
		specular = PearlSpecular;
		shininess = PearlShininess;
		Display();
		break;

		// metariał - rubin
	case RUBY:
		ambient = RubyAmbient;
		diffuse = RubyDiffuse;
		specular = RubySpecular;
		shininess = RubyShininess;
		Display();
		break;

		// materiał - turkus
	case TURQUOISE:
		ambient = TurquoiseAmbient;
		diffuse = TurquoiseDiffuse;
		specular = TurquoiseSpecular;
		shininess = TurquoiseShininess;
		Display();
		break;

		// materiał - czarny plastik
	case BLACK_PLASTIC:
		ambient = BlackPlasticAmbient;
		diffuse = BlackPlasticDiffuse;
		specular = BlackPlasticSpecular;
		shininess = BlackPlasticShininess;
		Display();
		break;

		// materiał - czarna guma
	case BLACK_RUBBER:
		ambient = BlackRubberAmbient;
		diffuse = BlackRubberDiffuse;
		specular = BlackRubberSpecular;
		shininess = BlackRubberShininess;
		Display();
		break;

		// swiatlo kierunkowe
	case SWIATLO_KIERUNKOWE:
		activity = SK;
		Display();
		break;

		// swiatlo otaczajace
	case SWIATLO_OTACZAJACE:
		activity = SO;
		Display();
		break;

		// reflektor
	case REFLEKTOR:
		activity = RE;
		Display();
		break;

		// wektory normalne - GLU_SMOOTH
	case NORMALS_SMOOTH:
		activity = WN;
		normals = NORMALS_SMOOTH;
		Display();
		break;

		// wektory normalne - GLU_FLAT
	case NORMALS_FLAT:
		activity = WN;
		normals = NORMALS_FLAT;
		Display();
		break;

		// obszar renderingu - całe okno
	case FULL_WINDOW:
		aspect = FULL_WINDOW;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// obszar renderingu - aspekt 1:1
	case ASPECT_1_1:
		aspect = ASPECT_1_1;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// wyjście
	case EXIT:
		exit(0);
	}
}

// sprawdzenie i przygotowanie obsługi wybranych rozszerzeñ

void ExtensionSetup()
{
	// pobranie numeru wersji biblioteki OpenGL
	const char *version = (char*)glGetString(GL_VERSION);

	// odczyt wersji OpenGL
	int major = 0, minor = 0;
	if (sscanf(version, "%d.%d", &major, &minor) != 2)
	{
#ifndef WIN32
		printf("Błędny format wersji OpenGL\n");
#else

		printf("Bledny format wersji OpenGL\n");
#endif

		exit(0);
	}

	// sprawdzenie czy jest co najmniej wersja 1.2
	if (major > 1 || minor >= 2)
		rescale_normal = true;
	else
		// sprawdzenie czy jest obsługiwane rozszerzenie EXT_rescale_normal
	if (glutExtensionSupported("GL_EXT_rescale_normal"))
		rescale_normal = true;
}

int main(int argc, char *argv[])
{
	// inicjalizacja biblioteki GLUT
	glutInit(&argc, argv);

	// inicjalizacja bufora ramki
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// rozmiary głównego okna programu
	glutInitWindowSize(500, 500);

	// utworzenie głównego okna programu
	glutCreateWindow("Zadanie 1");

	// dołączenie funkcji generującej scenę 3D
	glutDisplayFunc(Display);

	// dołączenie funkcji wywoływanej przy zmianie rozmiaru okna
	glutReshapeFunc(Reshape);

	// dołączenie funkcji obsługi klawiatury
	glutKeyboardFunc(Keyboard);

	// dołączenie funkcji obsługi klawiszy funkcyjnych i klawiszy kursora
	glutSpecialFunc(SpecialKeys);

	// obsługa przycisków myszki
	glutMouseFunc(MouseButton);

	// obsługa ruchu kursora myszki
	glutMotionFunc(MouseMotion);

	// utworzenie menu podręcznego
	glutCreateMenu(Menu);

	// utworzenie podmenu - Materiał
	int MenuMaterial = glutCreateMenu(Menu);

	glutAddMenuEntry("Mosiądz", BRASS);
	glutAddMenuEntry("Brąz", BRONZE);
	glutAddMenuEntry("Polerowany brąz", POLISHED_BRONZE);
	glutAddMenuEntry("Chrom", CHROME);
	glutAddMenuEntry("Miedź", COPPER);
	glutAddMenuEntry("Polerowana miedź", POLISHED_COPPER);
	glutAddMenuEntry("Złoto", GOLD);
	glutAddMenuEntry("Polerowane złoto", POLISHED_GOLD);
	glutAddMenuEntry("Grafit (cyna z ołowiem)", PEWTER);
	glutAddMenuEntry("Srebro", SILVER);
	glutAddMenuEntry("Polerowane srebro", POLISHED_SILVER);
	glutAddMenuEntry("Szmaragd", EMERALD);
	glutAddMenuEntry("Jadeit", JADE);
	glutAddMenuEntry("Obsydian", OBSIDIAN);
	glutAddMenuEntry("Perła", PEARL);
	glutAddMenuEntry("Rubin", RUBY);
	glutAddMenuEntry("Turkus", TURQUOISE);
	glutAddMenuEntry("Czarny plastik", BLACK_PLASTIC);
	glutAddMenuEntry("Czarna guma", BLACK_RUBBER);

	// utworzenie podmenu - Wektory normalne
	int MenuNormals = glutCreateMenu(Menu);

	glutAddMenuEntry("Jeden wektor normalny na wierzchołek", NORMALS_SMOOTH);
	glutAddMenuEntry("Jeden wektor normalny na ścianę", NORMALS_FLAT);


	// utworzenie podmenu - aspekt obrazu
	int MenuAspect = glutCreateMenu(Menu);

	glutAddMenuEntry("Aspekt obrazu - cale okno", FULL_WINDOW);
	glutAddMenuEntry("Aspekt obrazu 1:1", ASPECT_1_1);

	// menu główne
	glutCreateMenu(Menu);

	glutAddSubMenu("Materiał", MenuMaterial);
	glutAddSubMenu("Wektory normalne", MenuNormals);
	glutAddMenuEntry("Swiatlo kierunkowe", SWIATLO_KIERUNKOWE);
	glutAddMenuEntry("Swiatlo otaczajace", SWIATLO_OTACZAJACE);
	glutAddMenuEntry("Reflektor", REFLEKTOR);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);

	glutAddMenuEntry("Wyjscie", EXIT);


	// określenie przycisku myszki obsługującej menu podręczne
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// sprawdzenie i przygotowanie obsługi wybranych rozszerzeñ
	ExtensionSetup();

	// wprowadzenie programu do obsługi pętli komunikatów
	glutMainLoop();
	return 0;
}