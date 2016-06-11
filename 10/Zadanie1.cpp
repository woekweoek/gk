#include "stdafx.h"
#include <GL/glut.h>
#include <GL/glu.h>
#include "glext.h"
#include <stdlib.h>
#include <stdio.h>
#include "colors.h"
#include "materials.h"


// sta�e do obs�ugi menu podr�cznego
enum
{

	// materia�y
	BRASS, // mosi�dz
	BRONZE, // br�z
	POLISHED_BRONZE, // polerowany br�z
	CHROME, // chrom
	COPPER, // mied�
	POLISHED_COPPER, // polerowana mied�
	GOLD, // z�oto
	POLISHED_GOLD, // polerowane z�oto
	PEWTER, // grafit (cyna z o�owiem)
	SILVER, // srebro
	POLISHED_SILVER, // polerowane srebro
	EMERALD, // szmaragd
	JADE, // jadeit
	OBSIDIAN, // obsydian
	PEARL, // per�a
	RUBY, // rubin
	TURQUOISE, // turkus
	BLACK_PLASTIC, // czarny plastik
	BLACK_RUBBER, // czarna guma

	// obszar renderingu
	FULL_WINDOW, // aspekt obrazu - ca�e okno
	ASPECT_1_1, // aspekt obrazu 1:1
	EXIT, // wyj�cie
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
const GLdouble bottom = -2.0;
const GLdouble top = 2.0;
const GLdouble near = 3.0;
const GLdouble far = 7.0;

// k�ty obrotu obiektu

GLfloat rotatex = 0.0;
GLfloat rotatey = 0.0;

// wska�nik naci�ni�cia lewego przycisku myszki

int button_state = GLUT_UP;

// po�o�enie kursora myszki

int button_x, button_y;

// wsp�czynnik skalowania

GLfloat scale = 1.0;

// w�a�ciwo�ci materia�u - domy�lnie mosi�dz

const GLfloat * ambient = BrassAmbient;
const GLfloat * diffuse = BrassDiffuse;
const GLfloat * specular = BrassSpecular;
GLfloat shininess = BrassShininess;

// metoda podzia�u powierzchni NURBS na wielok�ty

int sampling_method = GLU_PATH_LENGTH;

// spos�b renderowania powierzchni NURBS

int display_mode = GLU_FILL;

const int K = 4;

// wsp�rz�dne punkt�w kontrolnych powierzchni

GLfloat points[4 * K * 3] =
{
	-0.5, 0.0, 0.0,			-0.5, 1.6, 0.0,			0.5, -0.5, 0.0,			-0.5, 0.0, 0.0,
	-0.1, 0.0, 0.5,			-0.5, 0.3, 0.5,			-0.7, -0.3, 0.5,			-0.1, 0.0, 0.5,
	-0.5, 0.0, 1.0,			-0.5, 0.6, 1.0,			0.5, 0.6, 1.0,			-0.5, 0.0, 1.0,
	-0.6, 0.0, 1.5,			-0.4, 1.5, 1.5,			0.5, -0.6, 1.5,			-0.6, 0.0, 1.5,

};

// w�z�y

GLfloat knots[4 * 2] =
{
	0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0
};

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

	// w��czenie testu bufora g��boko�ci
	glEnable(GL_DEPTH_TEST);

	// przesuni�cie uk�adu wsp�rz�dnych obiektu do �rodka bry�y odcinania
	glTranslatef(0, 0, -(near + far) / 2);

	// obroty obiektu
	glRotatef(rotatex, 1.0, 0, 0);
	glRotatef(rotatey, 0, 1.0, 0);

	// skalowanie obiektu - klawisze "+" i "-"
	glScalef(scale, scale, scale);

	// w��czenie efekt�w o�wietlenia, gry renderowana jest wype�niona powierzchnia
	if (display_mode == GLU_FILL)
	{
		// w��czenie o�wietlenia
		glEnable(GL_LIGHTING);

		// w��czenie �wiat�a GL_LIGHT0 z parametrami domy�lnymi
		glEnable(GL_LIGHT0);

		// w��czenie automatycznej normalizacji wektor�w normalnych
		glEnable(GL_NORMALIZE);

		// w�a�ciwo�ci materia�u
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

		// w��czenie automatycznego generowania wektor�w normalnych
		glEnable(GL_AUTO_NORMAL);
	}

	// kolor kraw�dzi
	glColor3fv(Black);

	// utworzenie obiektu NURBS
	GLUnurbsObj * nurbs = gluNewNurbsRenderer();

	// pocz�tek definicji powierzchni NURBS
	gluBeginSurface(nurbs);

	// spos�b renderowania powierzchni NURBS
	gluNurbsProperty(nurbs, GLU_DISPLAY_MODE, display_mode);

	// metoda podzia�u powierzchni NURBS na wielok�ty
	gluNurbsProperty(nurbs, GLU_SAMPLING_METHOD, sampling_method);

	// narysowanie powierzchni
	gluNurbsSurface(nurbs, 8, knots, 8, knots, 4 * 3, 3, points, 4, 4, GL_MAP2_VERTEX_3);

	// koniec definicji powierzchni
	gluEndSurface(nurbs);

	// usuni�cie obiektu NURBS
	gluDeleteNurbsRenderer(nurbs);

	// wy��czenie automatycznego generowania wektor�w normalnych
	glDisable(GL_AUTO_NORMAL);

	// wy��czenie automatycznej normalizacji wektor�w normalnych
	glDisable(GL_NORMALIZE);

	// wy��czenie �wiat�a GL_LIGHT0
	glDisable(GL_LIGHT0);

	// wy�aczenie o�wietlenia
	glDisable(GL_LIGHTING);

	// kolor punkt�w
	glColor3fv(Blue);

	// rozxmiar punkt�w
	glPointSize(6.0);

	// narysowanie punkt�w kontrolnych
	glBegin(GL_POINTS);
	for (int i = 0; i < 4 * K; i++)
		glVertex3fv(points + i * 3);

	glEnd();

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
		scale += 0.05;
	else

		// klawisz -
	if (key == '-' && scale > 0.05)
		scale -= 0.05;

	// narysowanie sceny
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
		// GLU_DISPLAY_MODE
	case GLU_FILL:
	case GLU_OUTLINE_PATCH:
	case GLU_OUTLINE_POLYGON:
		display_mode = value;
		DisplayScene();
		break;

		// GLU_SAMPLING_METHOD
	case GLU_PATH_LENGTH:
	case GLU_PARAMETRIC_ERROR:
	case GLU_DOMAIN_DISTANCE:

		sampling_method = value;
		DisplayScene();
		break;


		// materia� - mosi�dz
	case BRASS:
		ambient = BrassAmbient;
		diffuse = BrassDiffuse;
		specular = BrassSpecular;
		shininess = BrassShininess;
		DisplayScene();
		break;

		// materia� - br�z
	case BRONZE:
		ambient = BronzeAmbient;
		diffuse = BronzeDiffuse;
		specular = BronzeSpecular;
		shininess = BronzeShininess;
		DisplayScene();
		break;

		// materia� - polerowany br�z
	case POLISHED_BRONZE:
		ambient = PolishedBronzeAmbient;
		diffuse = PolishedBronzeDiffuse;
		specular = PolishedBronzeSpecular;
		shininess = PolishedBronzeShininess;
		DisplayScene();
		break;

		// materia� - chrom
	case CHROME:
		ambient = ChromeAmbient;
		diffuse = ChromeDiffuse;
		specular = ChromeSpecular;
		shininess = ChromeShininess;
		DisplayScene();
		break;

		// materia� - mied�
	case COPPER:
		ambient = CopperAmbient;
		diffuse = CopperDiffuse;
		specular = CopperSpecular;
		shininess = CopperShininess;
		DisplayScene();
		break;

		// materia� - polerowana mied�
	case POLISHED_COPPER:
		ambient = PolishedCopperAmbient;
		diffuse = PolishedCopperDiffuse;
		specular = PolishedCopperSpecular;
		shininess = PolishedCopperShininess;
		DisplayScene();
		break;

		// materia� - z�oto
	case GOLD:
		ambient = GoldAmbient;
		diffuse = GoldDiffuse;
		specular = GoldSpecular;
		shininess = GoldShininess;
		DisplayScene();
		break;

		// materia� - polerowane z�oto
	case POLISHED_GOLD:
		ambient = PolishedGoldAmbient;
		diffuse = PolishedGoldDiffuse;
		specular = PolishedGoldSpecular;
		shininess = PolishedGoldShininess;
		DisplayScene();
		break;

		// materia� - grafit (cyna z o�owiem)
	case PEWTER:
		ambient = PewterAmbient;
		diffuse = PewterDiffuse;
		specular = PewterSpecular;
		shininess = PewterShininess;
		DisplayScene();
		break;

		// materia� - srebro
	case SILVER:
		ambient = SilverAmbient;
		diffuse = SilverDiffuse;
		specular = SilverSpecular;
		shininess = SilverShininess;
		DisplayScene();
		break;

		// materia� - polerowane srebro
	case POLISHED_SILVER:
		ambient = PolishedSilverAmbient;
		diffuse = PolishedSilverDiffuse;
		specular = PolishedSilverSpecular;
		shininess = PolishedSilverShininess;
		DisplayScene();
		break;

		// materia� - szmaragd
	case EMERALD:
		ambient = EmeraldAmbient;
		diffuse = EmeraldDiffuse;
		specular = EmeraldSpecular;
		shininess = EmeraldShininess;
		DisplayScene();
		break;

		// materia� - jadeit
	case JADE:
		ambient = JadeAmbient;
		diffuse = JadeDiffuse;
		specular = JadeSpecular;
		shininess = JadeShininess;
		DisplayScene();
		break;

		// materia� - obsydian
	case OBSIDIAN:
		ambient = ObsidianAmbient;
		diffuse = ObsidianDiffuse;
		specular = ObsidianSpecular;
		shininess = ObsidianShininess;
		DisplayScene();
		break;

		// materia� - per�a
	case PEARL:
		ambient = PearlAmbient;
		diffuse = PearlDiffuse;
		specular = PearlSpecular;
		shininess = PearlShininess;
		DisplayScene();
		break;

		// metaria� - rubin
	case RUBY:
		ambient = RubyAmbient;
		diffuse = RubyDiffuse;
		specular = RubySpecular;
		shininess = RubyShininess;
		DisplayScene();
		break;

		// materia� - turkus
	case TURQUOISE:
		ambient = TurquoiseAmbient;
		diffuse = TurquoiseDiffuse;
		specular = TurquoiseSpecular;
		shininess = TurquoiseShininess;
		DisplayScene();
		break;

		// materia� - czarny plastik
	case BLACK_PLASTIC:
		ambient = BlackPlasticAmbient;
		diffuse = BlackPlasticDiffuse;
		specular = BlackPlasticSpecular;
		shininess = BlackPlasticShininess;
		DisplayScene();
		break;

		// materia� - czarna guma
	case BLACK_RUBBER:
		ambient = BlackRubberAmbient;
		diffuse = BlackRubberDiffuse;
		specular = BlackRubberSpecular;
		shininess = BlackRubberShininess;
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
	glutCreateWindow("Powierzchnia NURBS");

	// do��czenie funkcji generuj�cej scen� 3D
	glutDisplayFunc(DisplayScene);

	// do��czenie funkcji wywo�ywanej przy zmianie rozmiaru okna
	glutReshapeFunc(Reshape);

	// do��czenie funkcji obs�ugi klawiatury
	glutKeyboardFunc(Keyboard);

	// obs�uga przycisk�w myszki
	glutMouseFunc(MouseButton);

	// obs�uga ruchu kursora myszki
	glutMotionFunc(MouseMotion);

	// utworzenie menu podr�cznego
	glutCreateMenu(Menu);

	// utworzenie podmenu - GLU_DISPLAY_MODE
	int MenuDisplayMode = glutCreateMenu(Menu);
	glutAddMenuEntry("GLU_FILL", GLU_FILL);
	glutAddMenuEntry("GLU_OUTLINE_PATCH", GLU_OUTLINE_PATCH);
	glutAddMenuEntry("GLU_OUTLINE_POLYGON", GLU_OUTLINE_POLYGON);

	// utworzenie podmenu - GLU_SAMPLING_METHOD
	int MenuSamplingMethod = glutCreateMenu(Menu);
	glutAddMenuEntry("GLU_PATH_LENGTH", GLU_PATH_LENGTH);
	glutAddMenuEntry("GLU_PARAMETRIC_ERROR", GLU_PARAMETRIC_ERROR);
	glutAddMenuEntry("GLU_DOMAIN_DISTANCE", GLU_DOMAIN_DISTANCE);

	// utworzenie podmenu - Materia�
	int MenuMaterial = glutCreateMenu(Menu);

	glutAddMenuEntry("Mosi�dz", BRASS);
	glutAddMenuEntry("Br�z", BRONZE);
	glutAddMenuEntry("Polerowany br�z", POLISHED_BRONZE);
	glutAddMenuEntry("Chrom", CHROME);
	glutAddMenuEntry("Mied�", COPPER);
	glutAddMenuEntry("Polerowana mied�", POLISHED_COPPER);
	glutAddMenuEntry("Z�oto", GOLD);
	glutAddMenuEntry("Polerowane z�oto", POLISHED_GOLD);
	glutAddMenuEntry("Grafit (cyna z o�owiem)", PEWTER);
	glutAddMenuEntry("Srebro", SILVER);
	glutAddMenuEntry("Polerowane srebro", POLISHED_SILVER);
	glutAddMenuEntry("Szmaragd", EMERALD);
	glutAddMenuEntry("Jadeit", JADE);
	glutAddMenuEntry("Obsydian", OBSIDIAN);
	glutAddMenuEntry("Per�a", PEARL);
	glutAddMenuEntry("Rubin", RUBY);
	glutAddMenuEntry("Turkus", TURQUOISE);
	glutAddMenuEntry("Czarny plastik", BLACK_PLASTIC);
	glutAddMenuEntry("Czarna guma", BLACK_RUBBER);

	// utworzenie podmenu - Aspekt obrazu
	int MenuAspect = glutCreateMenu(Menu);

	glutAddMenuEntry("Aspekt obrazu - ca�e okno", FULL_WINDOW);
	glutAddMenuEntry("Aspekt obrazu 1:1", ASPECT_1_1);

	// menu g��wne
	glutCreateMenu(Menu);
	glutAddSubMenu("GLU_DISPLAY_MODE", MenuDisplayMode);
	glutAddSubMenu("GLU_SAMPLING_METHOD", MenuSamplingMethod);

	glutAddSubMenu("Materia�", MenuMaterial);
	glutAddSubMenu("Aspekt obrazu", MenuAspect);
	glutAddMenuEntry("Wyj�cie", EXIT);


	// okre�lenie przycisku myszki obs�uguj�cej menu podr�czne
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// wprowadzenie programu do obs�ugi p�tli komunikat�w
	glutMainLoop();
	return 0;
}