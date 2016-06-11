# Importujemy wszystkie konieczne biblioteki:
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
import sys
# Deklarujemy wszystkie zmienne globalne
global xrot # warto?? obracania wg kierunku x
global yrot # warto?? obracania wg kierunku y
global ambient # o?wietlanie otaczaj?ce
global greencolor # kolor igie? choinki
global treecolor # kolor badyla choinki
global lightpos # Lokalizacja ?r?d?a ?wiat?a
# Procedura inicializacji
def init():
    global xrot # warto?? obracania wg kierunku x
    global yrot # warto?? obracania wg kierunku y
    global ambient # o?wietlanie otaczaj?ce
    global greencolor # kolor igie? choinki
    global treecolor # kolor badyla choinki
    global lightpos # Lokalizacja ?r?d?a ?wiat?a
    xrot = 0.0 # warto?? obracania wg kierunku x = 0
    yrot = 0.0 # warto?? obracania wg kierunku y = 0
    ambient = (1.0, 1.0, 1.0, 1) # Pierwsze trzy warto?ci okre?laj? kolor w postaci RGB, ostatnia - jaskrawo??
    greencolor = (0.2, 0.8, 0.0, 0.8) # kolor zielony dla igie?
    treecolor = (0.9, 0.6, 0.3, 0.8) # kolor br?zowy dla badyla
    lightpos = (1.0, 1.0, 1.0) # lokalizacja ?r?d?a ?wiat?a wg osi xyz
    glClearColor(0.5, 0.5, 0.5, 1.0) # kolor szary dla zarysowania pocz?tkowego
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0) # okre?lamy kraw?dzi rysowania poziomo a pionowo
    glRotatef(-90, 1.0, 0.0, 0.0) # przesuniemy si? wg osi ? na 90 stopni
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient) # okre?lamy bie??cy model o?wietlania
    glEnable(GL_LIGHTING) # w??czamy o?wietlanie
    glEnable(GL_LIGHT0) # w??czamy jedne ?r?d?o ?wiat?a
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos) # okre?lamy lokalizacj? ?r?d?a ?wiat?a
# procedura opracowania przycisk?w specjalnych
def specialkeys(key, x, y):
    global xrot
    global yrot
# dla przucisk?w ze strza?kami
    if key == GLUT_KEY_UP: # przycisk dog?ry
        xrot -= 2.0 # zmniejszajmy k?t obracania wg osi ?
    if key == GLUT_KEY_DOWN: # przucisk dodo?u
        xrot += 2.0 # zwi?kszajmy k?t obracania wg osi ?
    if key == GLUT_KEY_LEFT: # w lewo
        yrot -= 2.0 # zmniejszajmy k?t obracania wg osi Y
    if key == GLUT_KEY_RIGHT: # na prawo
        yrot += 2.0 # zwi?kszajmy k?t obracania wg osi Y
    glutPostRedisplay() # wywo?ujemy procedur? przerysowania
# Procedura przerysowania
def draw():
    global xrot
    global yrot
    global lightpos
    global greencolor
    global treecolor
    glClear(GL_COLOR_BUFFER_BIT) # oczyszczamy ekran i ??????? ????? ? zalewamy szarym kolorem
    glPushMatrix() # Zachowujemy bie??ce po?o?enie "kamery"
    glRotatef(xrot, 1.0, 0.0, 0.0) # Obracamy po osi X na wielko?? xrot
    glRotatef(yrot, 0.0, 1.0, 0.0) # Obracamy po osi Y na wielko?? yrot
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos) # ?r?d?o ?wiat?a obracamy razem z choink?
# Rysujemy badyl choinki
# Ustalamy materia?: rysowa? z 2 stron, ?wiat?o otaczaj?ce, kolor br?zowy
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, treecolor)
    glTranslatef(0.0, 0.0, -0.7) # Przesuniemy si? wzd?u? osi Z na -0.7
# Rysujemy ?????? cylinder z radiusem 0.1, wysoko?ci? 0.2
# Ostatnie dwie liczby okre?laj? ilo?? wielok?t?w
    glutSolidCylinder(0.1, 0.2, 20, 20)
# Rysujemy ga??zi choinki
# Ustalamy materia?: rysowa? z 2 stron, ?wiat?o otaczaj?ce, kolor zielony
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, greencolor)
    glTranslatef(0.0, 0.0, 0.2) # Przesuniemy si? wzd?u? osi Z na 0.2
# Rysujemy dolne ga??zi (sto?ek) z radiusem 0.5, wysoko?ci? 0.5
# Ostatnie dwie liczby okreslaj? ilo?? wielok?t?w
    glutSolidCone(0.5, 0.5, 20, 20)
    glTranslatef(0.0, 0.0, 0.3) # Przesuniemy si? wzd?u? osi Z na -0.3
    glutSolidCone(0.4, 0.4, 20, 20) # Sto?ek z radiusem 0.4, wysoko?? 0.4
    glTranslatef(0.0, 0.0, 0.3) # Przesuniemy si? wzd?u? osi Z na -0.3
    glutSolidCone(0.3, 0.3, 20, 20) # Sto?ek z radiusem 0.3, wysoko?? 0.3
    glTranslatef(0.0, 0.0, 0.2)
    glutSolidCone(0.2, 0.2, 20, 20) 
    glTranslatef(0.0, 0.0, 0.15)
    glutSolidCone(0.1, 0.1, 20, 20)
    glPopMatrix() # Wracamy zachowane po?o?enie "kamery"
    glutSwapBuffers() # Wyprowadzamy wszystko namalowane w pami?ci na ekran
# Tutaj zaczanamy wykonywa? program
# Wykorzystujemy podw?jne buforowanie oraz kolory w postaci RGB (Czerwony, Zielony, Niebieski)
glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB)
# Ustalamy pocz?tkowy rozmiar okna (szeroko??, wysoko??)
glutInitWindowSize(300, 300)
# Ustalamy pocz?tkowe po?o?enie okna stosunkowo lewego g?rnego k?ta ekranu
glutInitWindowPosition(50, 50)
# Inicjalizacja OpenGl
glutInit(sys.argv)
# Tworzymy okno z nag?owkiem "Happy New Year!"
glutCreateWindow(b"Zadanie1")
# Okre?lamy procedur? odpowiedzaln? za przerysowanie
glutDisplayFunc(draw)
# Okre?lamy procedur? odpowiedzialn? za opracowanie przycisk?w
glutSpecialFunc(specialkeys)
# Wywo?ujemy funkcj? inicjalizacji
init()
# Uruchomimamy g?uwn? p?tl?
glutMainLoop()