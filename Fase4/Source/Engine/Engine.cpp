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
#include <map>
#include <iterator>
#include <GL/glew.h>
#include <GL/glut.h>
#include <IL/il.h>
#include "tinyxml2.h"
using namespace tinyxml2;
using namespace std;
float px, py, pz;
float radius = 200;
float alpha = 0;
float beta = M_PI / 4;
int timebase = 0, frame = 0;
float elapsed_time;
int showTrajectories = 0;

/*-----------------------------------------------------------------------------------------*/
void buildRotMatrix(float *x, float *y, float *z, float *pos, float *m) {
	//Transformation matrix for the object
	m[0] = x[0]; m[1] = x[1]; m[2] = x[2]; m[3] = 0;
	m[4] = y[0]; m[5] = y[1]; m[6] = y[2]; m[7] = 0;
	m[8] = z[0]; m[9] = z[1]; m[10] = z[2]; m[11] = 0;
	m[12] = pos[0]; m[13] = pos[1]; m[14] = pos[2]; m[15] = 1;
}
void cross(float *a, float *b, float *res) {

	res[0] = a[1] * b[2] - a[2] * b[1];
	res[1] = a[2] * b[0] - a[0] * b[2];
	res[2] = a[0] * b[1] - a[1] * b[0];
}
void normalize(float *a) {

	float l = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
	a[0] = a[0] / l;
	a[1] = a[1] / l;
	a[2] = a[2] / l;
}
void multMatrixVector(float *m, float *v, float *res) {

	for (int j = 0; j < 4; ++j) {
		res[j] = 0;
		for (int k = 0; k < 4; ++k) {
			res[j] += v[k] * m[j * 4 + k];
		}
	}
}
void getCatmullRomPoint(float t, int *indices, const vector<float> controlPoints, float *res, float *deriv) {

	// catmull-rom matrix
	float m[4][4] = { { -0.5f,  1.5f, -1.5f,  0.5f },
	{ 1.0f, -2.5f,  2.0f, -0.5f },
	{ -0.5f,  0.0f,  0.5f,  0.0f },
	{ 0.0f,  1.0f,  0.0f,  0.0f } };

	// reset res and deriv
	res[0] = 0.0; res[1] = 0.0; res[2] = 0.0;
	deriv[0] = 0.0; deriv[1] = 0.0; deriv[2] = 0.0;

	float T[4] = { t*t*t, t*t, t, 1 };
	float Px[4] = { controlPoints[indices[0] * 3], controlPoints[indices[1] * 3], controlPoints[indices[2] * 3], controlPoints[indices[3] * 3] };
	float Py[4] = { controlPoints[(indices[0] * 3) + 1], controlPoints[(indices[1] * 3) + 1], controlPoints[(indices[2] * 3) + 1], controlPoints[(indices[3] * 3) + 1] };
	float Pz[4] = { controlPoints[(indices[0] * 3) + 2], controlPoints[(indices[1] * 3) + 2], controlPoints[(indices[2] * 3) + 2], controlPoints[(indices[3] * 3) + 2] };
	float dT[4] = { 3 * t*t, 2 * t, 1, 0 };

	// Compute A = M * P
	float Ax[4];
	float Ay[4];
	float Az[4];
	multMatrixVector(&(m[0][0]), Px, Ax);
	multMatrixVector(&(m[0][0]), Py, Ay);
	multMatrixVector(&(m[0][0]), Pz, Az);

	// Compute point res = T * A
	res[0] = Ax[0] * T[0] + Ax[1] * T[1] + Ax[2] * T[2] + Ax[3] * T[3];
	res[1] = Ay[0] * T[0] + Ay[1] * T[1] + Ay[2] * T[2] + Ay[3] * T[3];
	res[2] = Az[0] * T[0] + Az[1] * T[1] + Az[2] * T[2] + Az[3] * T[3];

	// compute deriv = T' * A
	deriv[0] = Ax[0] * dT[0] + Ax[1] * dT[1] + Ax[2] * dT[2] + Ax[3] * dT[3];
	deriv[1] = Ay[0] * dT[0] + Ay[1] * dT[1] + Ay[2] * dT[2] + Ay[3] * dT[3];
	deriv[2] = Az[0] * dT[0] + Az[1] * dT[1] + Az[2] * dT[2] + Az[3] * dT[3];

}
// given  global t, returns the point in the curve
void getGlobalCatmullRomPoint(float gt, const vector<float>& controlPoints, float *res, float *deriv) {

	int point_count = controlPoints.size() / 3;

	float t = gt * point_count; // this is the real global t
	int index = floor(t);  // which segment
	t = t - index; // where within  the segment

	// indices store the points
	int indices[4];
	indices[0] = (index + point_count - 1) % point_count;
	indices[1] = (indices[0] + 1) % point_count;
	indices[2] = (indices[1] + 1) % point_count;
	indices[3] = (indices[2] + 1) % point_count;

	getCatmullRomPoint(t, indices, controlPoints, res, deriv);
}
void renderCatmullRomCurve(const vector<float>& controlPoints) {

	float t;
	float res[3];
	float deriv[3];

	// desenhar a curva usando segmentos de reta - GL_LINE_LOOP
	glBegin(GL_LINE_LOOP);
	for (t = 0.0; t < 1.0; t += 0.0001) {
		getGlobalCatmullRomPoint(t, controlPoints, res, deriv);
		glVertex3f(res[0], res[1], res[2]);
	}
	glEnd();
}
/*-----------------------------------------------------------------------------------------*/
class Light {
public:
	int type; //0->point, 1->direction, 2->spot
	int number;
	float position[4], ambient[4], diffuse[4], specular[4], spotDirection[4], cutoff;
public:
	Light();
	Light(int type, int number, float *pos, float *amb, float *diff, float *spec, float *spot, float cutoff);
	~Light() {};
	void turnUpTheLight();
};
Light::Light(int type, int number, float *pos, float *amb, float *diff, float *spec, float *spot, float cutoff) {
	Light::type = type;
	Light::number = number;
	Light::position[0] = pos[0]; Light::position[1] = pos[1]; Light::position[2] = pos[2]; Light::position[3] = pos[3];
	Light::ambient[0] = amb[0]; Light::ambient[1] = amb[1]; Light::ambient[2] = amb[2]; Light::ambient[3] = amb[3];
	Light::diffuse[0] = diff[0]; Light::diffuse[1] = diff[1]; Light::diffuse[2] = diff[2]; Light::diffuse[3] = diff[3];
	Light::specular[0] = spec[0]; Light::specular[1] = spec[1]; Light::specular[2] = spec[2]; Light::specular[3] = spec[3];
	if (type == 2) {
		Light::spotDirection[0] = spot[0]; Light::spotDirection[1] = spot[1]; Light::spotDirection[2] = spot[2];
		Light::cutoff = cutoff;
	}
}
void Light::turnUpTheLight() {
	if (type == 1) position[3] = 0;
	else position[3] = 1;

	glLightfv(number + GL_LIGHT0, GL_POSITION, position);
	glLightfv(number + GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(number + GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(number + GL_LIGHT0, GL_SPECULAR, specular);

	if (type == 2) {
		glLightfv(number + GL_LIGHT0, GL_SPOT_DIRECTION, spotDirection);
		glLightf(number + GL_LIGHT0, GL_SPOT_CUTOFF, cutoff);
	}
}
vector<Light> lights;
/*-----------------------------------------------------------------------------------------*/
class Primitive {
public:
	GLuint normals;
	int coordNumber;
	GLuint texCoord;
	int vertexNumber;
	GLuint vertex;
public:
	Primitive();
	Primitive(int vertexNumber, GLuint vertex, GLuint normals, int coordNumber, GLuint texCoord);
	~Primitive() {};
};
Primitive::Primitive() {
	vertexNumber = 0;
	vertex = 0;
	normals = 0;
	coordNumber = 0;
	texCoord = 0;
}
Primitive::Primitive(int vertexNumber, GLuint vertex, GLuint normals, int coordNumber, GLuint texCoord) {
	Primitive::vertexNumber = vertexNumber;
	Primitive::vertex = vertex;
	Primitive::normals = normals;
	Primitive::coordNumber = coordNumber;
	Primitive::texCoord = texCoord;
}
map<string, Primitive> PrimitiveMap;
/*-----------------------------------------------------------------------------------------*/
class Model {
public:
	int texFlag; // 0->color, 1->texture
	string primitive;
	string texture;
	float ambient[4], diffuse[4], specular[4], emissive[4], shininess;
public:
	Model() {};
	Model(int texFlag, string primitive, string texture, float *amb, float *diff, float * spec, float *emiss, float shin);
	~Model() {};
	void drawModel();
};
Model::Model(int texFlag, string primitive, string texture, float *amb, float *diff, float * spec, float *emiss, float shin) {
	Model::texFlag = texFlag;
	if(texFlag == 1)
		Model::texture = texture;
	Model::primitive = primitive;
	Model::ambient[0] = amb[0]; Model::ambient[1] = amb[1]; Model::ambient[2] = amb[2]; Model::ambient[3] = amb[3];
	Model::diffuse[0] = diff[0]; Model::diffuse[1] = diff[1]; Model::diffuse[2] = diff[2]; Model::diffuse[3] = diff[3];
	Model::specular[0] = spec[0]; Model::specular[1] = spec[1]; Model::specular[2] = spec[2]; Model::specular[3] = spec[3];
	Model::emissive[0] = emiss[0]; Model::emissive[1] = emiss[1]; Model::emissive[2] = emiss[2]; Model::emissive[3] = emiss[3];
	Model::shininess = shin;
}
map<string, GLuint> TextureMap;

/*-----------------------------------------------------------------------------------------*/
class Transformation {
public:
	float time, angle, x, y, z;

public:
	virtual void transform() {
	};
	Transformation() {};
	virtual ~Transformation() {};
};
class Translate : public Transformation {
public:
	int with_time;
	float step;
	vector<float> controlPoints;
public:
	Translate(float x, float y, float z, vector<float> points, float time, int with_time);
	void transform();
};
Translate::Translate(float x, float y, float z, vector<float> points, float time, int with_time) {
	Translate::with_time = with_time;
	if (with_time == 1) {
		Translate::controlPoints = points;
		Translate::time = time;
		Translate::step = 1 / (time * 1000);
	}
	Translate::x = x;
	Translate::y = y;
	Translate::z = z;
}
void Translate::transform() {
	if (with_time == 0)
		glTranslatef(x, y, z);
	else {
		float res[3];
		float deriv[3];
		if (showTrajectories)
			renderCatmullRomCurve(controlPoints);
		elapsed_time = glutGet(GLUT_ELAPSED_TIME);
		getGlobalCatmullRomPoint(elapsed_time*step, controlPoints, res, deriv);
		glTranslatef(res[0], res[1], res[2]);
	}
}
class Rotate : public Transformation {
public:
	int with_time;
public:
	Rotate(float angle, float x, float y, float z, float time, int with_time);
	void transform();
};
Rotate::Rotate(float angle, float x, float y, float z, float time, int with_time) {
	Rotate::with_time = with_time;
	Rotate::time = time;
	if (with_time == 0)
		Rotate::angle = angle;
	else
		Rotate::angle = 360 / (time * 1000);
	Rotate::x = x;
	Rotate::y = y;
	Rotate::z = z;
}
void Rotate::transform() {
	if (with_time == 0)
		glRotatef(angle, x, y, z);
	else {
		elapsed_time = glutGet(GLUT_ELAPSED_TIME);
		glRotatef(elapsed_time*angle, x, y, z);
	}
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
	vector<Model> models;
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
int storeLights(XMLElement * store_elem);
int storeRotate(Group &store_group, XMLElement * store_elem);
int storeTranslate(Group &store_group, XMLElement * store_elem);
int getTranslatePoints(vector<float>& cPoints, XMLElement* translateElem);
int storeScale(Group &store_group, XMLElement * store_elem);
int storeModels(Group &store_group, XMLElement * store_elem);
void loadTexture(string textureName);
void loadModel(string fileName);
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

	while (elem != nullptr) {
		if (strcmp(elem->Name(), "group") == 0) {
			Group childGroup;
			return_value *= loadGroup(childGroup, elem);
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
		else if (strcmp(elem->Name(), "lights") == 0)
			return_value *= storeLights( elem);

		if (return_value != 1) return return_value;
		elem = elem->NextSiblingElement();
	}
	return 1;
}
int storeLights(XMLElement * store_elem) {
	int number = 0;
	
	XMLElement * lightList = store_elem->FirstChildElement("light");
	if (lightList == nullptr) return XML_ERROR_PARSING_ELEMENT;

	while (lightList != nullptr) {
		//Default values
		int type = 0;
		float position[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
		float ambient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		float diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float spotDirection[3] = { 0.0f, 0.0f, -1.0f };
		float cutoff = 180.0f;

		const XMLAttribute *lightAttribute = lightList->FirstAttribute();
		if (!lightAttribute) return XML_ERROR_PARSING_ATTRIBUTE;

		while (lightAttribute != nullptr) {
			if (strcmp(lightAttribute->Name(), "type") == 0)
				if (strcmp(lightAttribute->Value(), "DIRECTIONAL") == 0)
					type = 1;
				else if (strcmp(lightAttribute->Value(), "SPOTLIGHT") == 0)
					type = 2;
			if (strcmp(lightAttribute->Name(), "diffR") == 0)
				diffuse[0] = stof(lightAttribute->Value());
			if (strcmp(lightAttribute->Name(), "diffG") == 0)
				diffuse[1] = stof(lightAttribute->Value());
			if (strcmp(lightAttribute->Name(), "diffB") == 0)
				diffuse[2] = stof(lightAttribute->Value());

			if (strcmp(lightAttribute->Name(), "specR") == 0)
				specular[0] = stof(lightAttribute->Value());
			if (strcmp(lightAttribute->Name(), "specG") == 0)
				specular[1] = stof(lightAttribute->Value());
			if (strcmp(lightAttribute->Name(), "specB") == 0)
				specular[2] = stof(lightAttribute->Value());

			if (strcmp(lightAttribute->Name(), "posX") == 0)
				position[0] = stof(lightAttribute->Value());
			if (strcmp(lightAttribute->Name(), "posY") == 0)
				position[1] = stof(lightAttribute->Value());
			if (strcmp(lightAttribute->Name(), "posZ") == 0)
				position[2] = stof(lightAttribute->Value());

			if (strcmp(lightAttribute->Name(), "ambR") == 0)
				ambient[0] = stof(lightAttribute->Value());
			if (strcmp(lightAttribute->Name(), "ambG") == 0)
				ambient[1] = stof(lightAttribute->Value());
			if (strcmp(lightAttribute->Name(), "ambB") == 0)
				ambient[2] = stof(lightAttribute->Value());

			if (strcmp(lightAttribute->Name(), "spotDirX") == 0)
				spotDirection[0] = stof(lightAttribute->Value());
			if (strcmp(lightAttribute->Name(), "spotDirY") == 0)
				spotDirection[1] = stof(lightAttribute->Value());
			if (strcmp(lightAttribute->Name(), "spotDirZ") == 0)
				spotDirection[2] = stof(lightAttribute->Value());

			if (strcmp(lightAttribute->Name(), "cutoff") == 0)
				cutoff = stof(lightAttribute->Value());

			lightAttribute = lightAttribute->Next();
		}

		Light light(type,number,position,ambient,diffuse,specular,spotDirection,cutoff);
		lights.push_back(light);
		glEnable(number + GL_LIGHT0);
		number++;

		lightList = lightList->NextSiblingElement();
	}

	return 1;
}
int storeRotate(Group &store_group, XMLElement * store_elem) {
	int with_time = 0;
	float time = 0, angle = 0, x = 0, y = 0, z = 0;
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
		else if (strcmp((attribute->Name()), "time") == 0 || strcmp((attribute->Name()), "TIME") == 0) {
			time = stof(attribute->Value());
			with_time = 1;
		}
		attribute = attribute->Next();
	}
	Rotate *rotate = new Rotate(angle, x, y, z, time, with_time);
	store_group.transformations.push_back(rotate);
	return 1;
}
int storeTranslate(Group &store_group, XMLElement * store_elem) {
	int with_time = 0;
	float time = 0;
	float x = 0, y = 0, z = 0;
	vector<float> controlPoints;
	const XMLAttribute *attribute = store_elem->FirstAttribute();
	if (!attribute) return XML_ERROR_PARSING_ATTRIBUTE;

	while (attribute != nullptr) {
		if (strcmp((attribute->Name()), "X") == 0 || strcmp((attribute->Name()), "x") == 0)
			x = stof(attribute->Value());
		else if (strcmp((attribute->Name()), "Y") == 0 || strcmp((attribute->Name()), "y") == 0)
			y = stof(attribute->Value());
		else if (strcmp((attribute->Name()), "Z") == 0 || strcmp((attribute->Name()), "z") == 0)
			z = stof(attribute->Value());
		else if (strcmp((attribute->Name()), "time") == 0 || strcmp((attribute->Name()), "TIME") == 0) {
			with_time = 1;
			time = stof(attribute->Value());
			if (!(getTranslatePoints(controlPoints, store_elem)))
				return 0;
		}
		attribute = attribute->Next();
	}
	Translate *translate = new Translate(x, y, z, controlPoints, time, with_time);
	store_group.transformations.push_back(translate);
	return 1;
}
int getTranslatePoints(vector<float> &cPoints, XMLElement* translateElem) {
	float x, y, z;
	XMLElement* pointElem = translateElem->FirstChildElement();
	if (pointElem == nullptr) return XML_ERROR_PARSING_ELEMENT;

	while (pointElem != nullptr) {
		x = 0; y = 0; z = 0;
		if (strcmp(pointElem->Name(), "point") == 0 || strcmp(pointElem->Name(), "Point") == 0) {

			const XMLAttribute *attribute = pointElem->FirstAttribute();
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
		}
		//Assim garantimos que as coordenadas são inseridas pela ordem correta.
		cPoints.push_back(x); cPoints.push_back(y); cPoints.push_back(z);
		pointElem = pointElem->NextSiblingElement();
	}
	if ((cPoints.size() / 3) < 4) {//garante que a catmull tem no minimo quatro pontos de controlo.
		cout << "Translate doesn't have enough control points." << endl;
		return 0;
	}
	return 1;
}
int storeScale(Group &store_group, XMLElement * store_elem) {
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
	Scale *scale = new Scale(x, y, z);
	store_group.transformations.push_back(scale);
	return 1;
}
void loadTexture(string textureName) {

	unsigned int t, tw, th;
	unsigned char *texData;
	unsigned int texID;

	if (TextureMap.find(textureName) == TextureMap.end()) { //Se a primitiva ainda não tiver sido inserida

		ilInit();
		ilEnable(IL_ORIGIN_SET);
		ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
		ilGenImages(1, &t);
		ilBindImage(t);
		ilLoadImage((ILstring)textureName.c_str());
		tw = ilGetInteger(IL_IMAGE_WIDTH);
		th = ilGetInteger(IL_IMAGE_HEIGHT);
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		texData = ilGetData();

		glGenTextures(1, &texID);

		glBindTexture(GL_TEXTURE_2D, texID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		TextureMap.insert(pair<string, GLuint>(textureName, texID));
		printf("Loaded texture: %s.\n",textureName);
	}
}
void loadModel(string fileName) {

	if (PrimitiveMap.find(fileName) == PrimitiveMap.end()) { //Se a primitiva ainda não tiver sido inserida
		ifstream reader(fileName);
		string x_string, y_string, z_string, x_norm_string, y_norm_string, z_norm_string, x_tex_string, y_tex_string;
		string line;
		vector<float> vertex_vec, norm_vec, coord_vec;

		if (!reader) {
			cout << "File " << fileName << " not found" << endl;
			return;
		}
		else {
			getline(reader, line);
			while (getline(reader, line)) {
				istringstream stream(line);
				stream >> x_string >> y_string >> z_string >> x_norm_string >> y_norm_string >> z_norm_string >> x_tex_string >> y_tex_string;
				vertex_vec.push_back(stof(x_string));
				vertex_vec.push_back(stof(y_string));
				vertex_vec.push_back(stof(z_string));
				norm_vec.push_back(stof(x_norm_string));
				norm_vec.push_back(stof(y_norm_string));
				norm_vec.push_back(stof(z_norm_string));
				coord_vec.push_back(stof(x_tex_string));
				coord_vec.push_back(stof(y_tex_string));
			}
		}
		GLuint vecbuf, normbuf, texbuf;
		glGenBuffers(1, &vecbuf);
		glBindBuffer(GL_ARRAY_BUFFER, vecbuf);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_vec.size(), &(vertex_vec[0]), GL_STATIC_DRAW);
		glGenBuffers(1, &normbuf);
		glBindBuffer(GL_ARRAY_BUFFER, normbuf);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * norm_vec.size(), &(norm_vec[0]), GL_STATIC_DRAW);
		glGenBuffers(1, &texbuf);
		glBindBuffer(GL_ARRAY_BUFFER, texbuf);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * coord_vec.size(), &(coord_vec[0]), GL_STATIC_DRAW);

		Primitive primitive(vertex_vec.size(), vecbuf, normbuf, coord_vec.size(), texbuf);
		PrimitiveMap.insert(pair<string, Primitive>(fileName, primitive));

		cout << "Loaded Primitive: " << fileName << endl;

	}
}
int storeModels(Group &store_group, XMLElement * store_elem) {
	const char* primitiveName;
	const char* textureName;
	int texFlag = 0;
	float amb[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float diff[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float spec[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float emiss[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shin = 0.0f;

	XMLElement * modelList = store_elem->FirstChildElement("model"); //Obter o primeiro elemento group
	if (modelList == nullptr) return XML_ERROR_PARSING_ELEMENT;

	while (modelList != nullptr) {
		const XMLAttribute *modelAttribute = modelList->FirstAttribute();
		if (!modelAttribute) return XML_ERROR_PARSING_ATTRIBUTE;

		while (modelAttribute != nullptr) {
			if (strcmp((modelAttribute->Name()), "file") == 0 || strcmp((modelAttribute->Name()), "FILE") == 0) {
				primitiveName = modelAttribute->Value();
				loadModel(primitiveName);
			}
			if (strcmp((modelAttribute->Name()), "texture") == 0 || strcmp((modelAttribute->Name()), "TEXTURE") == 0) {
				textureName = modelAttribute->Value();
				loadTexture(textureName);
				texFlag = 1;
			}
			if (strcmp(modelAttribute->Name(), "diffR") == 0) 
				diff[0] = stof(modelAttribute->Value());
			if (strcmp(modelAttribute->Name(), "diffG") == 0)
				diff[1] = stof(modelAttribute->Value());
			if (strcmp(modelAttribute->Name(), "diffB") == 0)
				diff[2] = stof(modelAttribute->Value());

			if (strcmp(modelAttribute->Name(), "specR") == 0)
				spec[0] = stof(modelAttribute->Value());
			if (strcmp(modelAttribute->Name(), "specG") == 0)
				spec[1] = stof(modelAttribute->Value());
			if (strcmp(modelAttribute->Name(), "specB") == 0)
				spec[2] = stof(modelAttribute->Value());

			if (strcmp(modelAttribute->Name(), "emisR") == 0)
				emiss[0] = stof(modelAttribute->Value());
			if (strcmp(modelAttribute->Name(), "emisG") == 0)
				emiss[1] = stof(modelAttribute->Value());
			if (strcmp(modelAttribute->Name(), "emisB") == 0)
				emiss[2] = stof(modelAttribute->Value());

			if (strcmp(modelAttribute->Name(), "ambR") == 0)
				amb[0] = stof(modelAttribute->Value());
			if (strcmp(modelAttribute->Name(), "ambG") == 0)
				amb[1] = stof(modelAttribute->Value());
			if (strcmp(modelAttribute->Name(), "ambB") == 0)
				amb[2] = stof(modelAttribute->Value());

			if (strcmp(modelAttribute->Name(), "shininess") == 0)
				shin = stof(modelAttribute->Value());

			modelAttribute = modelAttribute->Next();
		}
		if (texFlag == 0)
			textureName = "";
		Model model(texFlag, primitiveName, textureName, amb, diff, spec, emiss, shin);
		store_group.models.push_back(model); // push model object
		modelList = modelList->NextSiblingElement("model");
	}
	return 1;
}
void Model::drawModel() {
	Primitive primitive_to_draw = PrimitiveMap.at(primitive);
	GLuint texture_to_draw;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_EMISSION, emissive);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);


	glBindBuffer(GL_ARRAY_BUFFER, primitive_to_draw.vertex);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, primitive_to_draw.normals);
	glNormalPointer(GL_FLOAT, 0, 0);
	if(texFlag == 1){
		texture_to_draw = TextureMap.at(texture);
		glBindTexture(GL_TEXTURE_2D, texture_to_draw);
		glBindBuffer(GL_ARRAY_BUFFER, primitive_to_draw.texCoord);
		glTexCoordPointer(2, GL_FLOAT, 0, 0);
	}
	//  Draw
	glDrawArrays(GL_TRIANGLES, 0, (primitive_to_draw.vertexNumber / 3));
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Group::drawGroup() {
	glPushMatrix();
	for (auto &transformation : transformations) transformation->transform();
	for (auto &model : models) model.drawModel();
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
	float fps;
	int time;
	char s[64];

	px = radius * cos(beta) * sin(alpha);
	py = radius * sin(beta);
	pz = radius * cos(beta) * cos(alpha);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	
	gluLookAt(px, py, pz,
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);

	for (auto light : lights) light.turnUpTheLight();
	
	scene.drawGroup();

	frame++;
	time = glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		fps = frame*1000.0 / (time - timebase);
		timebase = time;
		frame = 0;
		sprintf(s, "FPS: %f6.2", fps);
		glutSetWindowTitle(s);
	}
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
	case 5:
		(showTrajectories == 0) ? showTrajectories = 1 : showTrajectories = 0;
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
	glutAddMenuEntry("SEE/HIDE PLANET TRAJECTORY", 5);

	// attach the menu to the right button
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
void initGL() {

	// alguns settings para OpenGL
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}
int main(int argc, char **argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(20, 20);
	glutInitWindowSize(600, 600);
	glutCreateWindow("engine_v4.0");
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		cout << "Glew está com problemas" << endl;
		cout << glewGetErrorString(err) << endl;
		return 0;
	}
	ilInit();
	int successXML = loadXML();
	if (!successXML) {
		cout << "Error loading XML." << endl;
		return 0;
	}

	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);

	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);

	createGLUTMenus();

	initGL();

	glutMainLoop();

	return 1;
}