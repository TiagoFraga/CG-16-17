#define _USE_MATH_DEFINES

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <math.h>



using namespace std;

struct Point {
	float coordenadas[3]; //array coordenadas[x,y,z];
};

float quadrantes[8][3] = {
	{ 1, 1, 1 },	//0-cima positivo positivo
	{ 1, 1,-1 },	//1-cima positivo negativo
	{ -1, 1,-1 },	//2-cima negativo negativo
	{ -1, 1, 1 },	//3-cima negativo positivo
	{ 1,-1, 1 },	//4-baixo positivo positivo
	{ 1,-1,-1 },	//5-baixo positivo negativo
	{ -1,-1,-1 },	//6-baixo negativo negativo
	{ -1,-1, 1 },	//7-baixo negativo positivo
};

int writeToFile(const char* fileName, const vector<Point> &pontos) {
	int wasGenerated;
	string string_pontos = "";

	string_pontos.append(to_string(pontos.size()));
	string_pontos.append("\n");

	for (const Point &ponto : pontos) {
		string_pontos.append(to_string(ponto.coordenadas[0]));
		string_pontos.append(", ");
		string_pontos.append(to_string(ponto.coordenadas[1]));
		string_pontos.append(", ");
		string_pontos.append(to_string(ponto.coordenadas[2]));
		string_pontos.append("\n");
	}

	ofstream writer(fileName);

	if (!writer) {
		cout << "File can't be created." << endl;
		wasGenerated = 0;
	}
	else {
		writer << string_pontos;
		writer.close();
		wasGenerated = 1;
	}
	return wasGenerated;
}

int generateModelPlane(float size, char* fileName) {
	vector<Point> pontos;
	Point vertice;
	//sequencia de quadrantes pela qual devem ser gerados os pontos
	int sequencia_quadrantes[6] = { 0, 1, 3, 3, 1, 2 };
	//Variavel que guarda em qual quadrante está
	int inQuadrante;

	for (int i = 0; i < 6; i++) {
		inQuadrante = sequencia_quadrantes[i];
		vertice.coordenadas[0] = (size / 2)*quadrantes[inQuadrante][0];
		vertice.coordenadas[1] = 0;
		vertice.coordenadas[2] = (size / 2)*quadrantes[inQuadrante][2];
		pontos.push_back(vertice);
	}

	return writeToFile(fileName, pontos);
}

int generateModelBox(float xdim, float ydim, float zdim, char* fileName) {
	vector<Point> pontos;
	Point vertice;
	int sequencia_quadrantes[36] = {
		0,1,3,3,1,2, //face superior
		5,4,6,6,4,7, //face inferior
		5,1,4,4,1,0, //face lateral direita
		7,3,6,6,3,2, //face lateral esquerda
		4,0,7,7,0,3, //face frontal
		6,2,5,5,2,1, //face traseira
	};
	int inQuadrante;

	for (int i = 0; i < 36; i++) {
		// gerar face superior
		inQuadrante = sequencia_quadrantes[i];
		vertice.coordenadas[0] = (xdim / 2)*quadrantes[inQuadrante][0];
		vertice.coordenadas[1] = (ydim / 2)*quadrantes[inQuadrante][1];
		vertice.coordenadas[2] = (zdim / 2)*quadrantes[inQuadrante][2];
		pontos.push_back(vertice);
	}

	return writeToFile(fileName, pontos);
}

int generateModelSphere(float radius, unsigned int slices, unsigned int stacks, char *fileName) {
	vector<Point> pontos;
	Point vertice, vertice_comum1, vertice_comum2;
	int stack, slice;
	float angulo_camada = (float)(M_PI / stacks);
	float angulo_fatia = (float)(2 * M_PI / slices);
	float beta = (float)-(M_PI / 2.0f);
	float alpha = 0;
	float nextBeta = beta + angulo_camada;
	float nextAlpha = alpha + angulo_fatia;

	for (stack = 0; stack < 5; stack++) {
		for (slice = 0; slice < slices; slice++) {

			vertice.coordenadas[0] = radius *cos(nextBeta)* sin(alpha);
			vertice.coordenadas[1] = radius * sin(nextBeta);
			vertice.coordenadas[2] = radius *cos(nextBeta)* cos(alpha);
			pontos.push_back(vertice);

			vertice_comum1.coordenadas[0] = radius *cos(beta) *sin(alpha);
			vertice_comum1.coordenadas[1] = radius * sin(beta);
			vertice_comum1.coordenadas[2] = radius *cos(beta) * cos(alpha);
			pontos.push_back(vertice_comum1);

			vertice_comum2.coordenadas[0] = radius *cos(nextBeta)* sin(nextAlpha);
			vertice_comum2.coordenadas[1] = radius * sin(nextBeta);
			vertice_comum2.coordenadas[2] = radius *cos(nextBeta)* cos(nextAlpha);
			pontos.push_back(vertice_comum2);

			pontos.push_back(vertice_comum2);
			pontos.push_back(vertice_comum1);

			vertice.coordenadas[0] = radius *cos(beta)* sin(nextAlpha);
			vertice.coordenadas[1] = radius * sin(beta);
			vertice.coordenadas[2] = radius *cos(beta)* cos(nextAlpha);
			pontos.push_back(vertice);

			alpha = nextAlpha;
			nextAlpha += angulo_fatia;
		}
		beta = nextBeta;
		nextBeta += angulo_camada;
	}

	return writeToFile(fileName, pontos);
}

int generateModelCone(float radius, float height,  unsigned int slices, unsigned int stacks, char *fileName) {
	vector<Point> pontos;
	Point vertice, vertice_comum1, vertice_comum2;
	int stack, slice;
	float tamanho_camada = (float)(height / stacks);
	float angulo_fatia = (float)(2 * M_PI / slices);
	float decremento_raio = radius / (float)stacks;
	float alpha = 0, camada = -height/2, raio = radius;
	float nextAlpha = alpha + angulo_fatia,
		nextCamada = camada + tamanho_camada,
		nextRaio = raio - decremento_raio;

	//Desenhar cone
	for (int stack = 0; stack < stacks; stack++) {
		for (int slice = 0; slice < slices; slice++) {
			vertice.coordenadas[0] = raio * sin(alpha);
			vertice.coordenadas[1] = camada;
			vertice.coordenadas[2] = raio * cos(alpha);
			pontos.push_back(vertice);

			vertice_comum1.coordenadas[0] = nextRaio * sin(alpha);
			vertice_comum1.coordenadas[1] = nextCamada;
			vertice_comum1.coordenadas[2] = nextRaio * cos(alpha);
			pontos.push_back(vertice_comum1);

			vertice_comum2.coordenadas[0] = raio * sin(nextAlpha);
			vertice_comum2.coordenadas[1] = camada;
			vertice_comum2.coordenadas[2] = raio * cos(nextAlpha);
			pontos.push_back(vertice_comum2);

			pontos.push_back(vertice_comum2);
			pontos.push_back(vertice_comum1);

			vertice.coordenadas[0] = nextRaio * sin(nextAlpha);
			vertice.coordenadas[1] = nextCamada;
			vertice.coordenadas[2] = nextRaio * cos(nextAlpha);
			pontos.push_back(vertice);
			alpha = nextAlpha;
			nextAlpha += angulo_fatia;
		}
		raio = nextRaio;
		nextRaio -= decremento_raio;
		camada = nextCamada;
		nextCamada += tamanho_camada;
	}

	//Desenhar baixo
	for (int slice = 0; slice <= slices; slice++) {
		alpha = slice * angulo_fatia;
		nextAlpha = (slice + 1) * angulo_fatia;

		vertice.coordenadas[0] = radius * sin(-alpha);
		vertice.coordenadas[1] = -height / 2;
		vertice.coordenadas[2] = radius * cos(-alpha);
		pontos.push_back(vertice);
		
		vertice.coordenadas[0] = 0;
		vertice.coordenadas[1] = -height / 2;
		vertice.coordenadas[2] = 0;
		pontos.push_back(vertice);

		vertice.coordenadas[0] = radius * sin(-nextAlpha);
		vertice.coordenadas[1] = -height / 2;
		vertice.coordenadas[2] = radius * cos(-nextAlpha);
		pontos.push_back(vertice);
	}
	return writeToFile(fileName, pontos);
}

int modelNumber(string modelName) {
	int model = 0;

	if (modelName.compare("plane") == 0) {
		model = 1;
	}
	else if (modelName.compare("box") == 0) {
		model = 2;
	}
	else if (modelName.compare("sphere") == 0) {
		model = 3;
	}
	else if (modelName.compare("cone") == 0) {
		model = 4;
	}
	return model;
}


int generateByModel(int argc, int model, char **argv) {
	int wasGenerated;
	switch (model)
	{
	case 0:
		cout << "Model doesn't exit." << endl;
		wasGenerated = 0;
		break;
	case 1:
		if (argc > 2)
			wasGenerated = generateModelPlane(2.0, argv[2]);
		else
			cout << "Not enough model arguments." << endl;
		break;

	case 2:
		if (argc > 5)
			wasGenerated = generateModelBox(stof(argv[2]), stof(argv[3]), stof(argv[4]), argv[5]);
		else
			cout << "Not enough model arguments." << endl;
		break;

	case 3:
		if (argc > 5)
			wasGenerated = generateModelSphere(stof(argv[2]), stoi(argv[3]), stoi(argv[4]), argv[5]);
		else
			cout << "Not enough model arguments." << endl;
		break;
		case 4:
		if(argc > 6)
		wasGenerated = generateModelCone(stof(argv[2]), stoi(argv[3]), stoi(argv[4]), stoi(argv[5]), argv[6]);
		else
		cout << "Not enough model arguments." << endl;
		break;
	default:
		break;
	}

	return wasGenerated;
}

int main(int argc, char *argv[]) {
	int model;
	int isGenerated;

	if (argc == 1) {
		cout << "Not enough arguments." << endl;
	}
	else {
		model = modelNumber(argv[1]);
		isGenerated = generateByModel(argc, model, argv);
		if (isGenerated)
			cout << "The model file was generated." << endl;
		else
			cout << "Model file not generated." << endl;
	}

	return 0;
}
