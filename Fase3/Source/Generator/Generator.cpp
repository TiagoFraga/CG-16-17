#define _USE_MATH_DEFINES

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <math.h>
#define X 0
#define Y 1
#define Z 2
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
//Função que cria o ficheiro que contém os pontos de controlo para os translates
int writeCurvePointsToFile(const char* fileName, const vector<float>& points) {
	int wasGenerated;
	string string_points = "";

	for (int i = 0; i < points.size(); i += 3) {
		string_points.append("<point x=\"");
		string_points.append(to_string(points[i]));
		string_points.append("\" y=\"");
		string_points.append(to_string(points[i + 1]));
		string_points.append("\" z=\"");
		string_points.append(to_string(points[i + 2]));
		string_points.append("\"/>\n");
	}

	ofstream writer(fileName);

	if (!writer) {
		cout << "File can't be created." << endl;
		wasGenerated = 0;
	}
	else {
		writer << string_points;
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

Point bezierMatrixFormula(vector<Point>& patch_cpoints, float u, float v) {
	// B(u,v) = U * M * Pxyz * Mt * V
	Point generated_point;
	float M[16] = { -1, 3, -3, 1, 3, -6, 3, 0, -3, 3, 0, 0, 1, 0, 0, 0 };
	float U[4] = { u*u*u, u*u, u, 1 };
	float V[4] = { v*v*v, v*v, v, 1 };
	float UM[4];
	float MV[4];
	float UMpatch_pointsX[4];
	float UMpatch_pointsY[4];
	float UMpatch_pointsZ[4];

	//U x M
	for (int i = 0; i < 4; i++)
		UM[i] = U[0] * M[i * 4 + 0] + U[1] * M[i * 4 + 1] + U[2] * M[i * 4 + 2] + U[3] * M[i * 4 + 3];
	//M x V
	for (int i = 0; i < 4; i++)
		MV[i] = M[i * 4 + 0] * V[0] + M[i * 4 + 1] * V[1] + M[i * 4 + 2] * V[2] + M[i * 4 + 3] * V[3];
	//UM x coordenadas X dos pontos de controlo do patch
	for (int i = 0; i < 4; i++)
		UMpatch_pointsX[i] = UM[0] * patch_cpoints[i * 4 + 0].coordenadas[X] + UM[1] * patch_cpoints[i * 4 + 1].coordenadas[X]
							+ UM[2] * patch_cpoints[i * 4 + 2].coordenadas[X] + UM[3] * patch_cpoints[i * 4 + 3].coordenadas[X];
		//UM x coordenadas Y dos pontos de controlo do patch
	for (int i = 0; i < 4; i++)
		UMpatch_pointsY[i] = UM[0] * patch_cpoints[i * 4 + 0].coordenadas[Y] + UM[1] * patch_cpoints[i * 4 + 1].coordenadas[Y]
							+ UM[2] * patch_cpoints[i * 4 + 2].coordenadas[Y] + UM[3] * patch_cpoints[i * 4 + 3].coordenadas[Y];
	//UM x coordenadas Z dos pontos de controlo do patch
	for (int i = 0; i < 4; i++)
		UMpatch_pointsZ[i] = UM[0] * patch_cpoints[i * 4 + 0].coordenadas[Z] + UM[1] * patch_cpoints[i * 4 + 1].coordenadas[Z]
							+ UM[2] * patch_cpoints[i * 4 + 2].coordenadas[Z] + UM[3] * patch_cpoints[i * 4 + 3].coordenadas[Z];
	
	//UMcoordenadasXYZ x MV
	generated_point.coordenadas[X] = UMpatch_pointsX[0] * MV[0] + UMpatch_pointsX[1] * MV[1] + UMpatch_pointsX[2] * MV[2] + UMpatch_pointsX[3] * MV[3];
	generated_point.coordenadas[Y] = UMpatch_pointsY[0] * MV[0] + UMpatch_pointsY[1] * MV[1] + UMpatch_pointsY[2] * MV[2] + UMpatch_pointsY[3] * MV[3];
	generated_point.coordenadas[Z] = UMpatch_pointsZ[0] * MV[0] + UMpatch_pointsZ[1] * MV[1] + UMpatch_pointsZ[2] * MV[2] + UMpatch_pointsZ[3] * MV[3];
	
	return generated_point;
}

int generateModelBezier(const char* inputFile, int tessellation, const char* outputFile) {
	vector<Point> bezier_points;
	ifstream input(inputFile);
	string line;
	int nr_patches = 0, nr_points = 0;
	vector<int> patches;
	vector<Point> control_points;

	if (!input) {
		cout << "Patch file can't be open" << endl;
		return 0;
	}
	else {
		//obter o numero de patches
		getline(input,line);
		nr_patches = stoi(line);
		//percorrer todas as linhas de patches
		for (int i = 0; i < nr_patches; i++) {
			getline(input, line);
			//Parsing de uma linha com os indices dos patches
			char* c_line = new char[line.size() +1];
			copy(line.begin(), line.end(), c_line);
			c_line[line.size()] = '\0';
			char* indice = strtok(c_line, " ,");
			while (indice) {
				patches.push_back(atoi(indice));
				indice = strtok(NULL, " ,");
			}
		}
		//obter o numero de pontos
		getline(input, line);
		nr_points = stoi(line);
		//percorrer todas as linhas de pontos
		for (int i = 0; i < nr_points; i++) {
			getline(input, line);
			Point p;
			//Parsing de uma linha com as coordenadas dos pontos de controlo
			char* c_line = new char[line.size() + 1];
			copy(line.begin(), line.end(), c_line);
			c_line[line.size()] = '\0';
			char* indice = strtok(c_line, " ,");
			p.coordenadas[X] = atof(indice);
			indice = strtok(NULL, " ,");
			p.coordenadas[Y] = atof(indice);
			indice = strtok(NULL, " ,");
			p.coordenadas[Z] = atof(indice);
			control_points.push_back(p);
		}
		for(int patch = 0; patch < nr_patches; patch++){
			vector<Point> patch_cpoints;
			float tess = (float)1 / (float)tessellation;

			for (int p = 0; p < 16; p++) {
				patch_cpoints.push_back(control_points[patches[patch * 16 + p]]);
			}
						
			for(int i = 0 ; i < tessellation + 1; i++){
				for (int j = 0; j < tessellation + 1; j++) {
					float u = i*tess;
					float v = j * tess;
					Point generated_bezier_point;
					generated_bezier_point = bezierMatrixFormula(patch_cpoints,u,v);
					bezier_points.push_back(generated_bezier_point);
					generated_bezier_point = bezierMatrixFormula(patch_cpoints, u+tess, v);
					bezier_points.push_back(generated_bezier_point);
					generated_bezier_point = bezierMatrixFormula(patch_cpoints, u, v+tess);
					bezier_points.push_back(generated_bezier_point);
					bezier_points.push_back(generated_bezier_point);
					generated_bezier_point = bezierMatrixFormula(patch_cpoints, u + tess, v);
					bezier_points.push_back(generated_bezier_point);
					generated_bezier_point = bezierMatrixFormula(patch_cpoints, u+tess, v + tess);
					bezier_points.push_back(generated_bezier_point);
				}
			}
		}

	}
	cout << bezier_points.size() << endl;
	return writeToFile(outputFile, bezier_points);
}

//Função que gera os pontos de controlo para os translates do config.xml
int generateCatmullEllipse(float radius, float x_percent, float z_percent, char *fileName) {
	float slices = 8;
	float angulo_fatia = (float)(2 * M_PI / slices);
	float alpha = M_PI / 2;
	float x, y, z;
	vector<float> curvePoints;

	for (int i = 0; i < slices; i++) {
		x = y = z = 0;
		x = (radius * x_percent) * sin(alpha);
		y = 0.0;
		z = (radius * z_percent) * cos(alpha);
		curvePoints.push_back(x); curvePoints.push_back(y); curvePoints.push_back(z);
		alpha += angulo_fatia;
	}

	return writeCurvePointsToFile(fileName, curvePoints);

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
	else if (modelName.compare("bezier") == 0) {
		model = 5;
	}
	else if (modelName.compare("curve") == 0) {
		model = 6;
	}
	return model;
}


int generateByModel(int argc, int model, char **argv) {
	int wasGenerated = 0;
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
	case 5:
		if (argc > 4)
			wasGenerated = generateModelBezier(argv[2], stoi(argv[3]), argv[4]);
		else
			cout << "Not enough arguments for bezier." << endl;
		break;
	case 6:
		if (argc > 5)
			wasGenerated = generateCatmullEllipse(stof(argv[2]), stof(argv[3]), stof(argv[4]), argv[5]);
		else
			cout << "Not enough arguments to generate curve points." << endl;
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
