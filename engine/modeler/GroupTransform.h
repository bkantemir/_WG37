#pragma once
#include <stdlib.h>
#include "ModelBuilder1base.h"

class GroupTransform
{
public:
	float shift[4] = { 0,0,0,0 };
	float spin[3] = { 0,0,0 }; //in degrees
	float scale[3] = { 1,1,1 };
	std::string allign = "";
	std::string headZto = "";
	std::string onThe = ""; //left/right/etc.
	//limit to
	Group01* pGroup = NULL; //NULL-all, can also be pCurrentGroup or pLastClosedGroup
	char mark[32] = ""; //must be in <>
	float pMin[3]{ -1000000 ,-1000000 ,-1000000 };
	float pMax[3]{ 1000000 , 1000000 , 1000000 };
public:
	int executeGroupTransform(ModelBuilder1base* pMB) { return executeGroupTransform(pMB, this); };
	static int executeGroupTransform(ModelBuilder1base* pMB, GroupTransform* pGT);
	//set limits
	static void limit2mark(GroupTransform* pGT, std::string mark0);
	static int flagSelection(GroupTransform* pGT, std::vector<Vertex01*>* pVertices, std::vector<Triangle01*>* pTriangles);
	static int cloneFlagged(ModelBuilder1base* pMB,
		std::vector<Vertex01*>* pVxDst, std::vector<Triangle01*>* pTrDst,
		std::vector<Vertex01*>* pVxSrc, std::vector<Triangle01*>* pTrSrc);
	static int refactorTriangles(std::vector<Triangle01*>* pTrDst, int trianglesN0dst, std::vector<Vertex01*>* pVxSrc);
	static int transformFlagged(GroupTransform* pGT, std::vector<Vertex01*>* pVxDst);
	static int transformFlaggedMx(std::vector<Vertex01*>* pVx, mat4x4* pTransformMatrix);

	static int buildTransformMatrix(GroupTransform* pGT, mat4x4* pTransformMatrix);
	static void flagAll(std::vector<Vertex01*>* pVx, std::vector<Triangle01*>* pTr);
	static int buildBoundingBoxFlagged(float* bbMin, float* bbMax, std::vector<Vertex01*>* pVx);
};

