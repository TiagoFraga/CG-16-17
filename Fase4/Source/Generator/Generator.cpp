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
struct Normal {
	float coordenadas[3]; //array coordenadas[x,y,z];
};
struct Textura {
	float coordenadas[2]; //array coordenadas[x,y];
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

float referencial[6][3] = {
	{0,1,0}, //y positivo
	{1,0,0}, //x positivo
	{0,0,1}, //z positivo
	{0,-1,0}, //y negativo
	{-1,0,0}, //x negativo
	{0,0,-1} //z negativo
};

float texturaPlana[4][2]{
	{1,0}, //canto direito baixo
	{1,1}, //canto direito cima
	{0,1}, //canto esquerdo cima
	{0,0} //canto esquerdo baixo
};
float texturaBox[14][2]{
	{0.25,0.00},
	{0.50,0.00},
	{0.00,0.33},
	{0.25,0.33},
	{0.00,0.66},
	{0.25,0.66},
	{0.50,0.33},
	{0.50,0.66},
	{0.75,0.33},
	{0.75,0.66},
	{1.00,0.33},
	{1.00,0.66},
	{0.25,1.00},
	{0.50,1.00}
};

int writeToFile(const char* fileName, const vector<Point> &pontos, const vector<Normal> &normais, 
																	const vector<Textura> &texture) {
	int wasGenerated;
	string string_pontos = "";

	string_pontos.append(to_string(pontos.size()));
	string_pontos.append("\n");

	for (int i = 0; i < pontos.size(); i++) {
		string_pontos.append(to_string(pontos[i].coordenadas[X]));
		string_pontos.append(", ");
		string_pontos.append(to_string(pontos[i].coordenadas[Y]));
		string_pontos.append(", ");
		string_pontos.append(to_string(pontos[i].coordenadas[Z]));
		string_pontos.append(", ");
		string_pontos.append(to_string(normais[i].coordenadas[X]));
		string_pontos.append(", ");
		string_pontos.append(to_string(normais[i].coordenadas[Y]));
		string_pontos.append(", ");
		string_pontos.append(to_string(normais[i].coordenadas[Z]));
		string_pontos.append(", ");
		string_pontos.append(to_string(texture[i].coordenadas[X]));
		string_pontos.append(", ");
		string_pontos.append(to_string(texture[i].coordenadas[Y]));
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
	vector<Normal> normais;
	vector<Textura> coordenadas_textura;
	Point vertice;
	Normal normal;
	Textura ponto_textura;
	//sequencia de quadrantes pela qual devem ser gerados os pontos
	int sequencia_quadrantes[6] = { 0, 1, 3, 3, 1, 2 };
	//Variavel que guarda em qual quadrante está
	int inQuadrante;

	for (int i = 0; i < 6; i++) {
		inQuadrante = sequencia_quadrantes[i];
		vertice.coordenadas[X] = (size / 2)*quadrantes[inQuadrante][X];
		vertice.coordenadas[Y] = 0;
		vertice.coordenadas[Z] = (size / 2)*quadrantes[inQuadrante][Z];
		pontos.push_back(vertice);
		normal.coordenadas[X] = referencial[0][X];
		normal.coordenadas[Y] = referencial[0][Y];
		normal.coordenadas[Z] = referencial[0][Z];
		normais.push_back(normal);
		ponto_textura.coordenadas[X] = texturaPlana[inQuadrante][X];
		ponto_textura.coordenadas[Y] = texturaPlana[inQuadrante][Y];
		coordenadas_textura.push_back(ponto_textura);

	}

	return writeToFile(fileName, pontos, normais, coordenadas_textura);
}

int generateModelBox(float xdim, float ydim, float zdim, char* fileName) {
	vector<Point> pontos;
	vector<Normal> normais;
	vector<Textura> texturas;
	Point vertice;
	Normal normal;
	Textura textura;
	int sequencia_quadrantes[36] = {
		0,1,3,3,1,2, //face superior
		5,4,6,6,4,7, //face inferior
		5,1,4,4,1,0, //face lateral direita
		7,3,6,6,3,2, //face lateral esquerda
		4,0,7,7,0,3, //face frontal
		6,2,5,5,2,1 //face traseira
	};
	int sequencia_normais[36] = {
		0,0,0,0,0,0, //y_positivo
		3,3,3,3,3,3, //y negativo
		1,1,1,1,1,1, //x positivo
		4,4,4,4,4,4, //x negativo
		2,2,2,2,2,2, //z positivo
		5,5,5,5,5,5 //z negativo
	};
	int sequencia_textura[36]{
		7,13,5,5,13,12,
		1,6,0,0,6,3,
		8,9,6,6,9,7,
		3,5,2,2,5,4,
		6,7,3,3,7,5,
		10,11,8,8,11,9,
	};
	int inQuadrante;
	int inNormal;
	int inTextura;

	for (int i = 0; i < 36; i++) {
		// gerar face superior
		inQuadrante = sequencia_quadrantes[i];
		inNormal = sequencia_normais[i];
		inTextura = sequencia_textura[i];
		vertice.coordenadas[X] = (xdim / 2)*quadrantes[inQuadrante][0];
		vertice.coordenadas[Y] = (ydim / 2)*quadrantes[inQuadrante][1];
		vertice.coordenadas[Z] = (zdim / 2)*quadrantes[inQuadrante][2];
		pontos.push_back(vertice);
		normal.coordenadas[X] = referencial[inNormal][X];
		normal.coordenadas[Y] = referencial[inNormal][Y];
		normal.coordenadas[Z] = referencial[inNormal][Z];
		normais.push_back(normal);
		textura.coordenadas[X] = texturaBox[inTextura][X];
		textura.coordenadas[Y] = texturaBox[inTextura][Y];
		texturas.push_back(textura);
	}

	return writeToFile(fileName, pontos, normais, texturas);
}

int generateModelSphere(float radius, unsigned int slices, unsigned int stacks, char *fileName) {
	vector<Point> pontos;
	vector<Normal> normais;
	vector<Textura> texturas;
	Point vertice, vertice_comum1, vertice_comum2;
	Normal normal, normal_comum1, normal_comum2;
	Textura textura, textura_comum1, textura_comum2;
	int stack, slice;
	float angulo_camada = (float)(M_PI / stacks);
	float angulo_fatia = (float)(2 * M_PI / slices);
	float beta = (float)-(M_PI / 2.0f);
	float alpha = 0;
	float nextBeta = beta + angulo_camada;
	float nextAlpha = alpha + angulo_fatia;
	float texture_stacks_step = 1.0 / (float) stacks;
	float texture_slices_step = 1.0 / (float)slices;

	for (stack = 0; stack < slices; stack++) {
		for (slice = 0; slice < slices; slice++) {

			vertice.coordenadas[X] = radius *cos(nextBeta)* sin(alpha);
			vertice.coordenadas[Y] = radius * sin(nextBeta);
			vertice.coordenadas[Z] = radius *cos(nextBeta)* cos(alpha);
			pontos.push_back(vertice);
			normal.coordenadas[X] = cos(nextBeta)* sin(alpha);
			normal.coordenadas[Y] = sin(nextBeta);
			normal.coordenadas[Z] = cos(nextBeta)* cos(alpha);
			normais.push_back(normal);
			textura.coordenadas[X] = slice * texture_slices_step;
			textura.coordenadas[Y] = (stack+1) * texture_stacks_step;
			texturas.push_back(textura);


			vertice_comum1.coordenadas[X] = radius *cos(beta) *sin(alpha);
			vertice_comum1.coordenadas[Y] = radius * sin(beta);
			vertice_comum1.coordenadas[Z] = radius *cos(beta) * cos(alpha);
			pontos.push_back(vertice_comum1);
			normal_comum1.coordenadas[X] = cos(beta) *sin(alpha);
			normal_comum1.coordenadas[Y] = sin(beta);
			normal_comum1.coordenadas[Z] = cos(beta) * cos(alpha);
			normais.push_back(normal_comum1);
			textura_comum1.coordenadas[X] = slice * texture_slices_step;
			textura_comum1.coordenadas[Y] = (stack) * texture_stacks_step;
			texturas.push_back(textura_comum1);

			vertice_comum2.coordenadas[X] = radius *cos(nextBeta)* sin(nextAlpha);
			vertice_comum2.coordenadas[Y] = radius * sin(nextBeta);
			vertice_comum2.coordenadas[Z] = radius *cos(nextBeta)* cos(nextAlpha);
			pontos.push_back(vertice_comum2);
			normal_comum2.coordenadas[X] = cos(nextBeta)* sin(nextAlpha);
			normal_comum2.coordenadas[Y] = sin(nextBeta);
			normal_comum2.coordenadas[Z] = cos(nextBeta)* cos(nextAlpha);
			normais.push_back(normal_comum2);
			textura_comum2.coordenadas[X] = (slice+1) * texture_slices_step;
			textura_comum2.coordenadas[Y] = (stack+1)* texture_stacks_step;
			texturas.push_back(textura_comum2);

			pontos.push_back(vertice_comum2);
			pontos.push_back(vertice_comum1);
			normais.push_back(normal_comum2);
			normais.push_back(normal_comum1);
			texturas.push_back(textura_comum2);
			texturas.push_back(textura_comum1);

			vertice.coordenadas[X] = radius *cos(beta)* sin(nextAlpha);
			vertice.coordenadas[Y] = radius * sin(beta);
			vertice.coordenadas[Z] = radius *cos(beta)* cos(nextAlpha);
			pontos.push_back(vertice);
			normal.coordenadas[X] = cos(beta)* sin(nextAlpha);
			normal.coordenadas[Y] = sin(beta);
			normal.coordenadas[Z] = cos(beta)* cos(nextAlpha);
			normais.push_back(normal);
			textura.coordenadas[X] = (slice+1) * texture_slices_step;
			textura.coordenadas[Y] = (stack) * texture_stacks_step;
			texturas.push_back(textura); 

			alpha = nextAlpha;
			nextAlpha += angulo_fatia;
		}
		beta = nextBeta;
		nextBeta += angulo_camada;
	}

	return writeToFile(fileName, pontos, normais, texturas);
}

int generateModelCone(float radius, float height,  unsigned int slices, unsigned int stacks, char *fileName) {
	vector<Point> pontos;
	vector<Normal> normais;
	vector<Textura> texturas;
	Point vertice, vertice_comum1, vertice_comum2;
	Normal normal, normal_comum1, normal_comum2;
	Textura textura, textura_comum1, textura_comum2;
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
			vertice.coordenadas[X] = raio * sin(alpha);
			vertice.coordenadas[Y] = camada;
			vertice.coordenadas[Z] = raio * cos(alpha);
			pontos.push_back(vertice);
			normal.coordenadas[X] = sin(alpha) / sqrt(sin(alpha)*sin(alpha) + 1 + cos(alpha)*cos(alpha));
			normal.coordenadas[Y] = 1 / sqrt(sin(alpha)*sin(alpha) + 1 + cos(alpha)*cos(alpha));
			normal.coordenadas[Z] = cos(alpha) / sqrt(sin(alpha)*sin(alpha) + 1 + cos(alpha)*cos(alpha));
			normais.push_back(normal);
			textura.coordenadas[X] = 0.5 + (raio * sin(alpha));
			textura.coordenadas[Y] = 0.5 + (raio * cos(alpha));
			texturas.push_back(textura);

			vertice_comum1.coordenadas[X] = nextRaio * sin(alpha);
			vertice_comum1.coordenadas[Y] = nextCamada;
			vertice_comum1.coordenadas[Z] = nextRaio * cos(alpha);
			pontos.push_back(vertice_comum1);
			normal_comum1.coordenadas[X] = sin(alpha) / sqrt(sin(alpha)*sin(alpha) + 1 + cos(alpha)*cos(alpha));
			normal_comum1.coordenadas[Y] = 1 / sqrt(sin(alpha)*sin(alpha) + 1 + cos(alpha)*cos(alpha));
			normal_comum1.coordenadas[Z] = cos(alpha) / sqrt(sin(alpha)*sin(alpha) + 1 + cos(alpha)*cos(alpha));
			normais.push_back(normal_comum1);
			textura_comum1.coordenadas[X] = 0.5 + (nextRaio * sin(alpha));
			textura_comum1.coordenadas[Y] = 0.5 + (nextRaio * cos(alpha));
			texturas.push_back(textura_comum1);

			vertice_comum2.coordenadas[X] = raio * sin(nextAlpha);
			vertice_comum2.coordenadas[Y] = camada;
			vertice_comum2.coordenadas[Z] = raio * cos(nextAlpha);
			pontos.push_back(vertice_comum2);
			normal_comum1.coordenadas[X] = sin(nextAlpha) / sqrt(sin(nextAlpha)*sin(nextAlpha) + 1 + cos(nextAlpha)*cos(nextAlpha));
			normal_comum1.coordenadas[Y] = 1 / sqrt(sin(nextAlpha)*sin(nextAlpha) + 1 + cos(nextAlpha)*cos(nextAlpha));
			normal_comum1.coordenadas[Z] = cos(nextAlpha) / sqrt(sin(nextAlpha)*sin(nextAlpha) + 1 + cos(nextAlpha)*cos(nextAlpha));
			normais.push_back(normal_comum2);
			textura_comum2.coordenadas[X] = 0.5 + (raio * sin(nextAlpha));
			textura_comum2.coordenadas[Y] = 0.5 + (raio * cos(nextAlpha));
			texturas.push_back(textura_comum2);

			pontos.push_back(vertice_comum2);
			pontos.push_back(vertice_comum1);
			normais.push_back(normal_comum2);
			normais.push_back(normal_comum1);
			texturas.push_back(textura_comum2);
			texturas.push_back(textura_comum1);

			vertice.coordenadas[X] = nextRaio * sin(nextAlpha);
			vertice.coordenadas[Y] = nextCamada;
			vertice.coordenadas[Z] = nextRaio * cos(nextAlpha);
			pontos.push_back(vertice);
			normal.coordenadas[X] = sin(nextAlpha) / sqrt(sin(nextAlpha)*sin(nextAlpha) + 1 + cos(nextAlpha)*cos(nextAlpha));
			normal.coordenadas[Y] = 1 / sqrt(sin(nextAlpha)*sin(nextAlpha) + 1 + cos(nextAlpha)*cos(nextAlpha));
			normal.coordenadas[Z] = cos(nextAlpha) / sqrt(sin(nextAlpha)*sin(nextAlpha) + 1 + cos(nextAlpha)*cos(nextAlpha));
			normais.push_back(normal);
			textura.coordenadas[X] = 0.5 + (nextRaio * sin(nextAlpha));
			textura.coordenadas[Y] = 0.5 + (nextRaio * cos(nextAlpha));
			texturas.push_back(textura);

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

		vertice.coordenadas[X] = radius * sin(-alpha);
		vertice.coordenadas[Y] = -height / 2;
		vertice.coordenadas[Z] = radius * cos(-alpha);
		pontos.push_back(vertice);
		normal.coordenadas[X] = 0;
		normal.coordenadas[Y] = -1;
		normal.coordenadas[Z] = 0;
		normais.push_back(normal);
		textura.coordenadas[X] = 0.5 + (radius * sin(-alpha));
		textura.coordenadas[Y] = 0.5 + (radius * cos(-alpha));
		texturas.push_back(textura);

		vertice.coordenadas[X] = 0;
		vertice.coordenadas[Y] = -height / 2;
		vertice.coordenadas[Z] = 0;
		pontos.push_back(vertice);
		normal.coordenadas[X] = 0;
		normal.coordenadas[Y] = -1;
		normal.coordenadas[Z] = 0;
		normais.push_back(normal);
		textura.coordenadas[X] = 0.5;
		textura.coordenadas[Y] = 0.5;
		texturas.push_back(textura);

		vertice.coordenadas[X] = radius * sin(-nextAlpha);
		vertice.coordenadas[Y] = -height / 2;
		vertice.coordenadas[Z] = radius * cos(-nextAlpha);
		pontos.push_back(vertice);
		normal.coordenadas[X] = 0;
		normal.coordenadas[Y] = -1;
		normal.coordenadas[Z] = 0;
		normais.push_back(normal);
		textura.coordenadas[X] = 0.5 + (radius * sin(-nextAlpha));
		textura.coordenadas[Y] = 0.5 + (radius * cos(-nextAlpha));
		texturas.push_back(textura);
	}
	return writeToFile(fileName, pontos, normais, texturas);
}
Point tangentUbezierMatrixFormula(vector<Point>& patch_cpoints, float u, float v) {
	// B(u,v) = U * M * Pxyz * Mt * V
	Point generated_point;
	float M[16] = { -1, 3, -3, 1, 3, -6, 3, 0, -3, 3, 0, 0, 1, 0, 0, 0 };
	float U[4] = { 3*u*u, 2*u, 1, 0 };
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
Point tangentVbezierMatrixFormula(vector<Point>& patch_cpoints, float u, float v) {
	// B(u,v) = U * M * Pxyz * Mt * V
	Point generated_point;
	float M[16] = { -1, 3, -3, 1, 3, -6, 3, 0, -3, 3, 0, 0, 1, 0, 0, 0 };
	float U[4] = { u*u*u, u*u, u, 1 };
	float V[4] = { 3*v*v, 2*v, 1, 0 };
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
Normal calculaVetorNormal(Point tu, Point tv) {
	float auxX, auxY, auxZ, length;
	Normal normal;

	//normalizar vetores tangentes U e V
	length = sqrt(tu.coordenadas[X]* tu.coordenadas[X] + tu.coordenadas[Y] * tu.coordenadas[Y] + tu.coordenadas[Z] * tu.coordenadas[Z]);
	tu.coordenadas[X] = tu.coordenadas[X] / length;
	tu.coordenadas[Y] = tu.coordenadas[Y] / length;
	tu.coordenadas[Z] = tu.coordenadas[Z] / length;

	length = sqrt(tv.coordenadas[X] * tv.coordenadas[X] + tv.coordenadas[Y] * tv.coordenadas[Y] + tv.coordenadas[Z] * tv.coordenadas[Z]);
	tv.coordenadas[X] = tv.coordenadas[X] / length;
	tv.coordenadas[Y] = tv.coordenadas[Y] / length;
	tv.coordenadas[Z] = tv.coordenadas[Z] / length;

	//cross-product
	auxX = tu.coordenadas[Y] * tv.coordenadas[Z] - tu.coordenadas[Z] * tv.coordenadas[Y];
	auxY = tu.coordenadas[Z] * tv.coordenadas[X] - tu.coordenadas[X] * tv.coordenadas[Z];
	auxZ = tu.coordenadas[X] * tv.coordenadas[Y] - tu.coordenadas[Y] * tv.coordenadas[X];

	//normalizar vetor normal
	length = sqrt(auxX*auxX + auxY*auxY + auxZ*auxZ);
	normal.coordenadas[X] = auxX / length;
	normal.coordenadas[Y] = auxY / length;
	normal.coordenadas[Z] = auxZ / length;

	return normal;

}
int generateModelBezier(const char* inputFile, int tessellation, const char* outputFile) {
	vector<Point> bezier_points;
	vector<Normal> normais;
	vector<Textura> texturas;
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
					Point tangent_u, tangent_v;
					Normal normal;
					Textura textura;

					generated_bezier_point = bezierMatrixFormula(patch_cpoints,u,v);
					bezier_points.push_back(generated_bezier_point);
					tangent_u = tangentUbezierMatrixFormula(patch_cpoints, u, v);
					tangent_v = tangentVbezierMatrixFormula(patch_cpoints, u, v);
					normal = calculaVetorNormal(tangent_u, tangent_v);
					normais.push_back(normal);
					textura.coordenadas[X] = u;
					textura.coordenadas[Y] = v;
					texturas.push_back(textura);

					generated_bezier_point = bezierMatrixFormula(patch_cpoints, u+tess, v);
					bezier_points.push_back(generated_bezier_point);
					tangent_u = tangentUbezierMatrixFormula(patch_cpoints, u + tess, v);
					tangent_v = tangentVbezierMatrixFormula(patch_cpoints, u + tess, v);
					normal = calculaVetorNormal(tangent_u, tangent_v);
					normais.push_back(normal);
					textura.coordenadas[X] = u+tess;
					textura.coordenadas[Y] = v;
					texturas.push_back(textura);

					generated_bezier_point = bezierMatrixFormula(patch_cpoints, u, v+tess);
					bezier_points.push_back(generated_bezier_point);
					tangent_u = tangentUbezierMatrixFormula(patch_cpoints, u, v + tess);
					tangent_v = tangentVbezierMatrixFormula(patch_cpoints, u, v + tess);
					normal = calculaVetorNormal(tangent_u, tangent_v);
					normais.push_back(normal);
					textura.coordenadas[X] = u;
					textura.coordenadas[Y] = v+tess;
					texturas.push_back(textura);

					bezier_points.push_back(generated_bezier_point);
					normais.push_back(normal);
					texturas.push_back(textura);

					generated_bezier_point = bezierMatrixFormula(patch_cpoints, u + tess, v);
					bezier_points.push_back(generated_bezier_point);
					tangent_u = tangentUbezierMatrixFormula(patch_cpoints, u + tess, v);
					tangent_v = tangentVbezierMatrixFormula(patch_cpoints, u + tess, v);
					normal = calculaVetorNormal(tangent_u, tangent_v);
					normais.push_back(normal);
					textura.coordenadas[X] = u+tess;
					textura.coordenadas[Y] = v;
					texturas.push_back(textura);

					generated_bezier_point = bezierMatrixFormula(patch_cpoints, u+tess, v + tess);
					bezier_points.push_back(generated_bezier_point);
					tangent_u = tangentUbezierMatrixFormula(patch_cpoints, u + tess, v + tess);
					tangent_v = tangentVbezierMatrixFormula(patch_cpoints, u + tess, v + tess);
					normal = calculaVetorNormal(tangent_u, tangent_v);
					normais.push_back(normal);
					textura.coordenadas[X] = u+tess;
					textura.coordenadas[Y] = v+tess;
					texturas.push_back(textura);
				}
			}
		}

	}
	cout << bezier_points.size() << endl;
	return writeToFile(outputFile, bezier_points, normais, texturas);
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
