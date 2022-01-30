#pragma once
#include "Coords.h"
#include "Material.h"
#include <string>

class GameSubj
{
public:
	std::string name;
	Coords ownCoords;
	Coords ownSpeed;
	float scale[3] = { 1,1,1 };
	int djStartN = 0; //first DJ N in DJs array (DrawJob::drawJobs)
	int djTotalN = 0; //number of DJs
	mat4x4 ownModelMatrix = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

	Material* pAltMaterial = NULL;

public:
	virtual ~GameSubj();
	void buildModelMatrix() { buildModelMatrix(this); };
	static void buildModelMatrix(GameSubj* pGS);
	virtual int moveSubj() { return moveSubj(this); };
	static int moveSubj(GameSubj* pGS);
};
