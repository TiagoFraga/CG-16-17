#ifndef XMLCheckResult
#define XMLCheckResult(a_eResult) if (a_eResult != XML_SUCCESS) { printf("Error: %i\n",a_eResult); return a_eResult; }
#endif
#define _USE_MATH_DEFINES

#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <string>
#include <vector>
#include <GL/glut.h>
#include "tinyxml2.h"

using namespace tinyxml2;
using namespace std;

float px, py, pz;
float radius = 5;
float alpha = 0;
float beta = 0;

typedef vector<string> Primitive; //vector que contem todos os pontos de uma primitiva
vector<Primitive> models; // Vector que contem todas as primitivas

int loadModels(vector<string>& list) {
	string line;

	for (auto const& fileName : list) {
		Primitive points;
		ifstream reader(fileName);
		if (!reader) {
			return 0;
		}
		else {
			while (getline(reader, line))
				points.push_back(line);
		}
		cout << "Loaded Primitive: " << fileName << endl;
		models.push_back(points);
	}
	return 1;

}

int loadXML() {
	vector<string> list; //lista que contem o nome dos ficheiros a desenhar
	XMLDocument doc;
	const char* modelFile;

	XMLError eResult = doc.LoadFile("config.xml");
	XMLCheckResult(eResult);

	XMLNode * pRoot = doc.RootElement(); //Obter o nodo scene
	if (pRoot == nullptr) return XML_ERROR_FILE_READ_ERROR;

	XMLElement * elementList = pRoot->FirstChildElement("model"); //Obter o primeiro elemento model
	if (elementList == nullptr) return XML_ERROR_PARSING_ELEMENT;

	while (elementList != nullptr) {
		//obter o atributo de model
		modelFile = elementList->Attribute("file");
		if (modelFile == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
		//colocar o valor do atributo no vector
		list.push_back(modelFile);
		//iterar para o próximo elemento.
		elementList = elementList->NextSiblingElement("model");
	}

	return loadModels(list);

}

void drawPrimitive(const Primitive& primitive) {
	string x_string, y_string, z_string;
	float x, y, z;
	int color = 0;
	int vertice_count;
	istringstream vertice_count_string(primitive.at(0));
	vertice_count_string >> vertice_count;

	glBegin(GL_TRIANGLES);
	for (int i = 1; i <= vertice_count; i++) {
		istringstream stream(primitive.at(i));
		stream >> x_string >> y_string >> z_string;
		x = stof(x_string);
		y = stof(y_string);
		z = stof(z_string);
		if ((i - 1) % 3 == 0) (color == 0) ? color = 1 : color = 0;
		if (color)
			glColor3f(0.7f, 0.9f, 0.3f);
		else
			glColor3f(0.4f, 0.9f, 0.7f);
		glVertex3f(x, y, z);
	}
	glEnd();
}


void changeSize(int w, int h) {

	if (h == 0)
		h = 1;
	float ratio = w * 1.0 / h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, w, h);

	gluPerspective(45.0f, ratio, 1.0f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);
}


void renderScene(void) {

	glClearColor(1.0, 1.0, 1.0, 1.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	px = radius * cos(beta) * sin(alpha);
	py = radius * sin(beta);
	pz = radius * cos(beta) * cos(alpha);

	glLoadIdentity();
	gluLookAt(px, py, pz,
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);

	for (auto const& primitive : models) {
		drawPrimitive(primitive);
	}

	glutSwapBuffers();
}

void processKeys(unsigned char c, int xx, int yy) {

	switch (c) {
	case 'm':
		radius += 0.05f;
		break;
	case 'n':
		radius -= 0.05f;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}


void processSpecialKeys(int key, int xx, int yy) {

	switch (key) {
	case GLUT_KEY_UP:
		if (beta < (M_PI / 2))
			beta += (float)M_PI / 50;
		break;
	case GLUT_KEY_DOWN:
		if (beta > -(M_PI / 2))
			beta -= (float)M_PI / 50;
		break;
	case GLUT_KEY_LEFT:
		alpha -= (float)M_PI / 50;
		break;
	case GLUT_KEY_RIGHT:
		alpha += (float)M_PI / 50;
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void processMenuEvents(int option) {
	switch (option) {
	case 1:
		glCullFace(GL_BACK);
		break;
	case 2:
		glCullFace(GL_FRONT);
		break;
	case 3:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 4:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void createGLUTMenus() {

	int menu;

	// create the menu and
	// tell glut that "processMenuEvents" will
	// handle the events
	menu = glutCreateMenu(processMenuEvents);

	//add entries to our menu
	glutAddMenuEntry("SEE FRONT", 1);
	glutAddMenuEntry("SEE BACK", 2);
	glutAddMenuEntry("SEE WIRED", 3);
	glutAddMenuEntry("SEE SOLID", 4);

	// attach the menu to the right button
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char **argv) {

	int successXML = loadXML();
	if (!successXML) cout << "Error reading XML file." << endl;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(20, 20);
	glutInitWindowSize(600, 600);
	glutCreateWindow("engine_v1.0");

	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);

	createGLUTMenus();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	glutMainLoop();

	return 1;
}