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
float radius = 200;
float alpha = 0;
float beta = M_PI / 4;
typedef vector<string> Primitive; //vector que contem todos os pontos de uma primitiva

/*-----------------------------------------------------------------------------------------*/
class Transformation {
public:
	float angle, x, y, z;
public:
	virtual void transform() {
	};
	Transformation() {};
	virtual ~Transformation() {};
};
class Translate : public Transformation {
public:
	Translate(float x, float y, float z);
	void transform();
};
Translate::Translate(float x, float y, float z){
	Translate::x = x;
	Translate::y = y;
	Translate::z = z;
}
void Translate::transform() {
	glTranslatef(x, y, z);
}
class Rotate : public Transformation {
public:
	Rotate(float angle, float x, float y, float z);
	void transform();
};
Rotate::Rotate(float angle, float x, float y, float z) {
	Rotate::angle = angle;
	Rotate::x = x;
	Rotate::y = y;
	Rotate::z = z;
}
void Rotate::transform() {
	glRotatef(angle, x, y, z);
}
class Scale : public Transformation {
public:
	Scale(float x, float y, float z);
	void transform();
};
Scale::Scale(float x, float y, float z) {
	Scale::x = x;
	Scale::y = y;
	Scale::z = z;
}
void Scale::transform() {
	glScalef(x, y, z);
}
/*-----------------------------------------------------------------------------------------*/
class Group {
public:
	vector<Transformation*> transformations;
	vector<Primitive> models;
	vector<Group> childgroups;
public:
	Group() {};
	~Group() {};
	void drawGroup();
};
Group scene;
/*-----------------------------------------------------------------------------------------*/
int loadXML();
int loadGroup(Group &main_group, XMLElement * root_elem);
int storeRotate(Group &store_group, XMLElement * store_elem);
int storeTranslate(Group &store_group, XMLElement * store_elem);
int storeScale(Group &store_group, XMLElement * store_elem);
int storeModels(Group &store_group, XMLElement * store_elem);
int loadModels(Group &group, vector<string>& list);
void drawPrimitive(const Primitive& primitive);
/*-----------------------------------------------------------------------------------------*/

int loadXML() {
	XMLDocument doc;

	XMLError eResult = doc.LoadFile("config.xml");
	XMLCheckResult(eResult);

	XMLNode * pRoot = doc.RootElement(); //Obter o nodo scene
	if (pRoot == nullptr) return XML_ERROR_FILE_READ_ERROR;

	XMLElement * groupList = pRoot->FirstChildElement("group"); //Obter o primeiro elemento group
	if (groupList == nullptr) return XML_ERROR_PARSING_ELEMENT;

	return loadGroup(scene, groupList);
}
int loadGroup(Group &main_group, XMLElement * root_elem) {
	int return_value = 1;
	XMLElement* elem = root_elem->FirstChildElement(); //retorna o primeiro elemento do grupo
	if (elem == nullptr) return XML_ERROR_PARSING_ELEMENT;

	while(elem != nullptr){
		if (strcmp(elem->Name(), "group") == 0) {
			Group childGroup;
			loadGroup(childGroup, elem);
			main_group.childgroups.push_back(childGroup);
		}
		else if (strcmp(elem->Name(), "translate") == 0)
			return_value *= storeTranslate(main_group, elem);
		else if (strcmp(elem->Name(), "rotate") == 0)
			return_value *= storeRotate(main_group, elem);
		else if (strcmp(elem->Name(), "scale") == 0)
			return_value *= storeScale(main_group, elem);
		else if (strcmp(elem->Name(), "models") == 0)
			return_value *= storeModels(main_group, elem);
		
		if (return_value != 1) return return_value;
		elem = elem->NextSiblingElement();
	}
	return 1;
}
int storeRotate(Group &store_group, XMLElement * store_elem) {
	float angle = 0, x = 0, y = 0, z = 0;
	const XMLAttribute *attribute = store_elem->FirstAttribute();
	if (!attribute) return XML_ERROR_PARSING_ATTRIBUTE;

	while (attribute != nullptr) {
		if (strcmp((attribute->Name()), "X") == 0 || strcmp((attribute->Name()), "x") == 0)
			x = stof(attribute->Value());
		else if (strcmp((attribute->Name()), "Y") == 0 || strcmp((attribute->Name()), "y") == 0)
			y = stof(attribute->Value());
		else if (strcmp((attribute->Name()), "Z") == 0 || strcmp((attribute->Name()), "z") == 0)
			z = stof(attribute->Value());
		else if (strcmp((attribute->Name()), "angle") == 0 || strcmp((attribute->Name()), "ANGLE") == 0)
			angle = stof(attribute->Value());
		attribute = attribute->Next();
	}
	Rotate *rotate = new Rotate(angle,x,y,z);
	store_group.transformations.push_back(rotate);
	return 1;
}
int storeTranslate(Group &store_group, XMLElement * store_elem) {
	float x = 0, y = 0, z = 0;
	const XMLAttribute *attribute = store_elem->FirstAttribute();
	if (!attribute) return XML_ERROR_PARSING_ATTRIBUTE;

	while (attribute != nullptr) {
		if (strcmp((attribute->Name()), "X") == 0 || strcmp((attribute->Name()), "x") == 0)
			x = stof(attribute->Value());
		else if (strcmp((attribute->Name()), "Y") == 0 || strcmp((attribute->Name()), "y") == 0)
			y = stof(attribute->Value());
		else if (strcmp((attribute->Name()), "Z") == 0 || strcmp((attribute->Name()), "z") == 0)
			z = stof(attribute->Value());
		attribute = attribute->Next();
	}
	Translate *translate = new Translate(x,y,z);
	store_group.transformations.push_back(translate);
	return 1;
}
int storeScale(Group &store_group, XMLElement * store_elem) {
	float x = 0, y = 0, z = 0;
	const XMLAttribute *attribute = store_elem->FirstAttribute();
	if (!attribute) return XML_ERROR_PARSING_ATTRIBUTE;

	while (attribute != nullptr) {
		if (strcmp((attribute->Name()), "X") == 0 || strcmp((attribute->Name()), "x") == 0)
			x = stof(attribute->Value());
		else if(strcmp((attribute->Name()), "Y") == 0 || strcmp((attribute->Name()), "y") == 0)
			y = stof(attribute->Value());
		else if(strcmp((attribute->Name()), "Z") == 0 || strcmp((attribute->Name()), "z") == 0)
			z = stof(attribute->Value());
		attribute = attribute->Next();
	}
	Scale *scale = new Scale(x,y,z);
	store_group.transformations.push_back(scale);
	return 1;
}
int storeModels(Group &store_group, XMLElement * store_elem) {
	const char* modelFile;
	vector<string> fileNameList;

	XMLElement * modelList = store_elem->FirstChildElement("model"); //Obter o primeiro elemento group
	if (modelList == nullptr) return XML_ERROR_PARSING_ELEMENT;

	while (modelList != nullptr) {
		modelFile = modelList->Attribute("file");
		if (modelFile == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
		fileNameList.push_back(modelFile);
		modelList = modelList->NextSiblingElement("model");
	}

	return loadModels(store_group, fileNameList);
}
int loadModels(Group &group, vector<string>& list) {
	string line;

	for (auto const& fileName : list) {
		Primitive primitive;
		ifstream reader(fileName);
		if (!reader) {
			return 0;
		}
		else {
			while (getline(reader, line))
				primitive.push_back(line);
		}
		cout << "Loaded Primitive: " << fileName << endl;
		group.models.push_back(primitive);
	}
	return 1;
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
void Group::drawGroup() {
	glPushMatrix();
	for (auto &transformation : transformations) transformation->transform();
	for (auto const &model : models) drawPrimitive(model);
	for (auto &child : childgroups) child.drawGroup();
	glPopMatrix();
}
/*-----------------------------------------------------------------------------------------*/
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

	px = radius * cos(beta) * sin(alpha);
	py = radius * sin(beta);
	pz = radius * cos(beta) * cos(alpha);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(px, py, pz,
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);

	scene.drawGroup();

	glutSwapBuffers();
}

void processKeys(unsigned char c, int xx, int yy) {

	switch (c) {
	case 'm':
		radius += 1.0f;
		break;
	case 'n':
		radius -= 1.0f;
		break;
	default:
		break;
	}
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
	if (!successXML) cout << "Error loading XML." << endl;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(20, 20);
	glutInitWindowSize(600, 600);
	glutCreateWindow("engine_v2.0");

	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);

	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);

	createGLUTMenus();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	glutMainLoop();

	return 1;
}

