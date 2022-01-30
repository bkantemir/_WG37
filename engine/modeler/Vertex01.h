#pragma once

class Vertex01
{
public:
	char marks[124] = "";
	int subjN = -1; //game subject number
	int materialN = -1; //material number
	int flag = 0;
	int endOfSequence = 0; //for sequentional (unindexed) primitives (like GL_LINE_STRIP for example)
	int altN = -1; //vertex' position in alternative array
	//atributes
	float aPos[4] = { 0,0,0,0 }; //position x,y,z + 4-th float for matrix operations
	float aNormal[4] = { 0,0,0,0 }; //normal (surface reflection vector) x,y,z + 4-th float for matrix operations
	float aTuv[2] = { 0,0 }; //2D texture coordinates
	float aTuv2[2] = { 0,0 }; //for normal maps
	//tangent space (for normal maps)
	float aTangent[3] = { 0,0,0 };
	float aBinormal[3] = { 0,0,0 };
};
