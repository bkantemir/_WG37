#include "ModelBuilder.h"
#include "platform.h"
#include "utils.h"
#include "DrawJob.h"
#include "Shader.h"

extern float degrees2radians;

ModelBuilder::~ModelBuilder() {
}

int ModelBuilder::buildFace(ModelBuilder* pMB, std::string applyTo, VirtualShape* pVS, TexCoords* pTC, TexCoords* pTC2nm) {
	if (strstr(pVS->shapeType, "box") == pVS->shapeType)
		return buildBoxFace(pMB, applyTo, pVS, pTC, pTC2nm);
	return -1;
}
int ModelBuilder::buildBoxFace(ModelBuilder* pMB, std::string applyTo, VirtualShape* pVS, TexCoords* pTC, TexCoords* pTC2nm) {
	//this code is for simple box
	VirtualShape vs; //face VS, 
	mat4x4 transformMatrix = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	vs.sectionsR = pVS->sectionsR;
	//rotate desirable side to face us. 
	if (applyTo.find("front") == 0) {
		//Side <front> is facing us as is.
		vs.whl[0] = pVS->whl[0];
		vs.whl[1] = pVS->whl[1];
		vs.sections[0] = pVS->sections[0];
		vs.sections[1] = pVS->sections[1];
		//extensions
		vs.extF = pVS->extF;
		vs.extL = pVS->extL;
		vs.extR = pVS->extR;
		vs.extU = pVS->extU;
		vs.extD = pVS->extD;
		//define how to move/place generated face back to the VirtualShape
		//just shift closer to us by length/2
		mat4x4_translate(transformMatrix, 0, 0, pVS->whl[2] / 2);
	}
	else if (applyTo.find("back") == 0) {
		vs.whl[0] = pVS->whl[0];
		vs.whl[1] = pVS->whl[1];
		vs.sections[0] = pVS->sections[0];
		vs.sections[1] = pVS->sections[1];
		//extensions
		vs.extF = pVS->extB;
		vs.extL = pVS->extR;
		vs.extR = pVS->extL;
		vs.extU = pVS->extU;
		vs.extD = pVS->extD;
		//rotate 180 degrees around Y and shift farther from us by half-length
		mat4x4_translate(transformMatrix, 0, 0, -pVS->whl[2] / 2);
		mat4x4_rotate_Y(transformMatrix, transformMatrix, degrees2radians * 180);
	}
	else if (applyTo.find("left") == 0) {
		vs.whl[0] = pVS->whl[2]; //width = original length
		vs.whl[1] = pVS->whl[1];
		vs.sections[0] = pVS->sections[2];
		vs.sections[1] = pVS->sections[1];
		//extensions
		vs.extF = pVS->extL;
		vs.extL = pVS->extB;
		vs.extR = pVS->extF;
		vs.extU = pVS->extU;
		vs.extD = pVS->extD;
		//rotate -90 degrees around Y (CW) and shift half-width to the left
		mat4x4_translate(transformMatrix, -pVS->whl[0] / 2, 0, 0);
		mat4x4_rotate_Y(transformMatrix, transformMatrix, -degrees2radians * 90);
	}
	else if (applyTo.find("right") == 0) {
		vs.whl[0] = pVS->whl[2]; //width = original length
		vs.whl[1] = pVS->whl[1];
		vs.sections[0] = pVS->sections[2];
		vs.sections[1] = pVS->sections[1];
		//extensions
		vs.extF = pVS->extR;
		vs.extL = pVS->extF;
		vs.extR = pVS->extB;
		vs.extU = pVS->extU;
		vs.extD = pVS->extD;
		//rotate +90 degrees around Y (CCW) and shift half-width to the right
		mat4x4_translate(transformMatrix, pVS->whl[0] / 2, 0, 0);
		mat4x4_rotate_Y(transformMatrix, transformMatrix, degrees2radians * 90);
	}
	else if (applyTo.find("top") == 0) {
		vs.whl[0] = pVS->whl[0];
		vs.whl[1] = pVS->whl[2]; //height = original length
		vs.sections[0] = pVS->sections[0];
		vs.sections[1] = pVS->sections[2];
		//extensions
		vs.extF = pVS->extU;
		vs.extL = pVS->extR;
		vs.extR = pVS->extL;
		vs.extU = pVS->extF;
		vs.extD = pVS->extB;
		//rotate -90 degrees around X (CW) and 180 around Y, and shift half-height up
		mat4x4_translate(transformMatrix, 0, pVS->whl[1] / 2, 0);
		mat4x4_rotate_Y(transformMatrix, transformMatrix, degrees2radians * 180);
		mat4x4_rotate_X(transformMatrix, transformMatrix, -degrees2radians * 90);
	}
	else if (applyTo.find("bottom") == 0) {
		vs.whl[0] = pVS->whl[0];
		vs.whl[1] = pVS->whl[2]; //height = original length
		vs.sections[0] = pVS->sections[0];
		vs.sections[1] = pVS->sections[2];
		//extensions
		vs.extF = pVS->extD;
		vs.extL = pVS->extL;
		vs.extR = pVS->extR;
		vs.extU = pVS->extF;
		vs.extD = pVS->extB;
		//rotate 90 around X (CCW) and shift half-height down
		mat4x4_translate(transformMatrix, 0, -pVS->whl[1] / 2, 0);
		mat4x4_rotate_X(transformMatrix, transformMatrix, degrees2radians * 90);
	}
	lockGroup(pMB);
	//create vertices
	if (strstr(pVS->shapeType, "tank") != nullptr)
		buildBoxFaceTank(pMB, applyTo, &vs);
	else
		buildBoxFacePlain(pMB, applyTo, &vs);

	groupApplyTexture(pMB, "front", pTC, pTC2nm);

	//move face to it's place (apply transform matrix)
	int vertsN = pMB->vertices.size();
	for (int i = pMB->pCurrentGroup->fromVertexN; i < vertsN; i++) {
		Vertex01* pVX = pMB->vertices.at(i);
		mat4x4_mul_vec4plus(pVX->aPos, transformMatrix, pVX->aPos, 1);
		mat4x4_mul_vec4plus(pVX->aNormal, transformMatrix, pVX->aNormal, 0);
	}
	releaseGroup(pMB);
	return 1;
}
int ModelBuilder::buildBoxFacePlain(ModelBuilder* pMB, std::string applyTo, VirtualShape* pVS) {
	if (pVS->whl[0] == 0 || pVS->whl[1] == 0)
		return 0;
	//create vertices
	int sectionsX = pVS->sections[0];
	int sectionsY = pVS->sections[1];
	int pointsX = sectionsX + 1;
	int pointsY = sectionsY + 1;
	float stepX = pVS->whl[0] / sectionsX;
	float stepY = pVS->whl[1] / sectionsY;
	float kY = pVS->whl[1] / 2;
	for (int iy = 0; iy < pointsY; iy++) {
		float kX = -pVS->whl[0] / 2;
		for (int ix = 0; ix < pointsX; ix++) {
			int nSE = addVertex(pMB, kX, kY, pVS->extF, 0, 0, 1); //vertex number on south-east
			if (iy > 0 && ix > 0) {
				//add 2 triangles
				int nSW = nSE - 1; //vertex number south-west
				int nNE = nSE - pointsX; //north-east
				int nNW = nSW - pointsX; //north-west
				add2triangles(pMB, nNW, nNE, nSW, nSE, iy + ix);
			}
			kX += stepX;
		}
		kY -= stepY;
	}
	return 1;
}
int ModelBuilder::buildBoxFaceTank(ModelBuilder* pMB, std::string applyTo, VirtualShape* pVS) {
	//for diamond effect - sectionsRad=1, don't merge normals
	bool drawMiddle = true;
	//edges
	bool drawTop = false;
	bool drawBottom = false;
	bool drawLeft = false;
	bool drawRight = false;
	//corners
	bool drawTopLeft = false;
	bool drawTopRight = false;
	bool drawBottomLeft = false;
	bool drawBottomRight = false;
	if (pVS->extF == 0 || applyTo.find(" all") != std::string::npos) {
		drawTop = true;
		drawBottom = true;
		drawLeft = true;
		drawRight = true;
		drawTopLeft = true;
		drawTopRight = true;
		drawBottomLeft = true;
		drawBottomRight = true;
	}
	else if (applyTo.find(" h") != std::string::npos) {
		drawLeft = true;
		drawRight = true;
	}
	else if (applyTo.find(" v") != std::string::npos) {
		drawTop = true;
		drawBottom = true;
	}
	if (applyTo.find(" no") != std::string::npos) {
		if (applyTo.find(" noM") != std::string::npos) {
			//middle
			if (applyTo.find(" noMrow") != std::string::npos) {
				drawMiddle = false;
				drawLeft = false;
				drawRight = false;
			}
			if (applyTo.find(" noMcol") != std::string::npos) {
				drawMiddle = false;
				drawTop = false;
				drawBottom = false;
			}
			if (applyTo.find(" noMid") != std::string::npos)
				drawMiddle = false;
		}
		if (applyTo.find(" noN") != std::string::npos) {
			//north
			if (applyTo.find(" noNrow") != std::string::npos) {
				drawTop = false;
				drawTopLeft = false;
				drawTopRight = false;
			}
			if (applyTo.find(" noNedge") != std::string::npos)
				drawTop = false;
			if (applyTo.find(" noNW") != std::string::npos)
				drawTopLeft = false;
			if (applyTo.find(" noNE") != std::string::npos)
				drawTopRight = false;
		}
		if (applyTo.find(" noS") != std::string::npos) {
			//south
			if (applyTo.find(" noSrow") != std::string::npos) {
				drawBottom = false;
				drawBottomLeft = false;
				drawBottomRight = false;
			}
			if (applyTo.find(" noSedge") != std::string::npos)
				drawBottom = false;
			if (applyTo.find(" noSW") != std::string::npos)
				drawBottomLeft = false;
			if (applyTo.find(" noSE") != std::string::npos)
				drawBottomRight = false;
		}
		if (applyTo.find(" noW") != std::string::npos) {
			//west
			if (applyTo.find(" noWcol") != std::string::npos) {
				drawLeft = false;
				drawTopLeft = false;
				drawBottomLeft = false;
			}
			if (applyTo.find(" noWedge") != std::string::npos)
				drawLeft = false;
		}
		if (applyTo.find(" noE") != std::string::npos) {
			//east
			if (applyTo.find(" noEcol") != std::string::npos) {
				drawRight = false;
				drawTopRight = false;
				drawBottomRight = false;
			}
			if (applyTo.find(" noEedge") != std::string::npos)
				drawRight = false;
		}
	}
	lockGroup(pMB);
	//middle
	if (pVS->whl[0] > 0 && pVS->whl[1] > 0 && drawMiddle) {
		buildBoxFacePlain(pMB, applyTo, pVS);
	}
	VirtualShape vs;
	//edges
	//vs.type.assign("cylinder");
	vs.sectionsR = pVS->sectionsR;
	if (pVS->whl[0] > 0) {
		vs.sections[2] = pVS->sections[0]; //cylinder Z sections n
		vs.whl[2] = pVS->whl[0]; //cylinder length Z
		vs.whl[0] = pVS->extF * 2; //cylinder diameter X
		if (pVS->extU > 0 && drawTop) {
			vs.whl[1] = pVS->extU * 2; //cylinder diameter Y
			lockGroup(pMB);
			cylinderWrap(pMB, &vs, 0, 90);
			//rotate -90 degrees around Y and shift up
			moveGroupDg(pMB, 0, -90, 0, 0, pVS->whl[1] * 0.5f, 0);
			releaseGroup(pMB);
		}
		if (pVS->extD > 0 && drawBottom) {
			vs.whl[1] = pVS->extD * 2; //cylinder diameter Y
			lockGroup(pMB);
			cylinderWrap(pMB, &vs, -90, 0);
			//rotate -90 degrees around Y and shift down
			moveGroupDg(pMB, 0, -90, 0, 0, -pVS->whl[1] * 0.5f, 0);
			releaseGroup(pMB);
		}
	}
	if (pVS->whl[1] > 0) {
		vs.sections[2] = pVS->sections[1]; //cylinder Z sections n
		vs.whl[2] = pVS->whl[1]; //cylinder length Z
		vs.whl[1] = pVS->extF * 2; //cylinder diameter Y
		if (pVS->extL > 0 && drawLeft) {
			vs.whl[0] = pVS->extL * 2; //cylinder diameter X
			lockGroup(pMB);
			cylinderWrap(pMB, &vs, 90, 180);
			//rotate 90 degrees around Y and shift left
			moveGroupDg(pMB, 90, 0, 0, -pVS->whl[0] * 0.5f, 0, 0);
			releaseGroup(pMB);
		}
		if (pVS->extR > 0 && drawRight) {
			vs.whl[0] = pVS->extR * 2; //cylinder diameter X
			lockGroup(pMB);
			cylinderWrap(pMB, &vs, 0, 90);
			//rotate 90 degrees around Y and shift left
			moveGroupDg(pMB, 90, 0, 0, pVS->whl[0] * 0.5f, 0, 0);
			releaseGroup(pMB);
		}
	}
	//corners
	//vs.type.assign("cap");
	vs.sectionsR = pVS->sectionsR;
	vs.sections[2] = pVS->sectionsR;
	vs.whl[2] = pVS->extF;
	if (pVS->extU > 0) {
		//top corners
		vs.whl[1] = pVS->extU * 2;
		if (pVS->extL > 0 && drawTopLeft) {
			vs.whl[0] = pVS->extL * 2;
			lockGroup(pMB);
			capWrap(pMB, &vs, 90, 180);
			//rotate 90 degrees around Y and shift left
			moveGroupDg(pMB, 0, 0, 0, -pVS->whl[0] * 0.5f, pVS->whl[1] * 0.5f, 0);
			releaseGroup(pMB);
		}
		if (pVS->extR > 0 && drawTopRight) {
			vs.whl[0] = pVS->extR * 2;
			lockGroup(pMB);
			capWrap(pMB, &vs, 0, 90);
			//rotate 90 degrees around Y and shift left
			moveGroupDg(pMB, 0, 0, 0, pVS->whl[0] * 0.5f, pVS->whl[1] * 0.5f, 0);
			releaseGroup(pMB);

		}
	}
	if (pVS->extD > 0) {
		//bottom corners
		vs.whl[1] = pVS->extD * 2;
		if (pVS->extL > 0 && drawBottomLeft) {
			vs.whl[0] = pVS->extL * 2;
			lockGroup(pMB);
			capWrap(pMB, &vs, -180, -90);
			//rotate 90 degrees around Y and shift left
			moveGroupDg(pMB, 0, 0, 0, -pVS->whl[0] * 0.5f, -pVS->whl[1] * 0.5f, 0);
			releaseGroup(pMB);
		}
		if (pVS->extR > 0 && drawBottomRight) {
			vs.whl[0] = pVS->extR * 2;
			lockGroup(pMB);
			capWrap(pMB, &vs, -90, 0);
			//rotate 90 degrees around Y and shift left
			moveGroupDg(pMB, 0, 0, 0, pVS->whl[0] * 0.5f, -pVS->whl[1] * 0.5f, 0);
			releaseGroup(pMB);
		}
	}
	if (pVS->extF == 0) {
		int vertsN = pMB->vertices.size();
		for (int i = pMB->pCurrentGroup->fromVertexN; i < vertsN; i++) {
			Vertex01* pVX = pMB->vertices.at(i);
			//normal
			v3set(pVX->aNormal, 0, 0, 1);
		}
	}
	releaseGroup(pMB);
	return 1;
}

int ModelBuilder::cylinderWrap(ModelBuilder* pMB, VirtualShape* pVS, float angleFrom, float angleTo) {
	// angleFrom/To - in degrees
	lockGroup(pMB);
	float stepZ = pVS->whl[2] / pVS->sections[2];
	float stepDg = (angleTo - angleFrom) / pVS->sectionsR; //in degrees
	for (int nz = 0; nz <= pVS->sections[2]; nz++) {
		float kz = stepZ * nz - pVS->whl[2] * 0.5f;
		for (int rpn = 0; rpn <= pVS->sectionsR; rpn++) {
			// rpn - radial point number
			float angleRd = (angleFrom + stepDg * rpn) * degrees2radians;
			float kx = cosf(angleRd);
			float ky = sinf(angleRd);
			int nSE = addVertex(pMB, kx, ky, kz, kx, ky, 0);
			if (nz > 0 && rpn > 0) {
				int nSW = nSE - 1;
				int nNW = nSW - pVS->sectionsR - 1;
				int nNE = nSE - pVS->sectionsR - 1;
				add2triangles(pMB, nNE, nNW, nSE, nSW, nz + rpn);
			}
		}
	}
	//scale to desirable diameters
	mat4x4 transformMatrix = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	mat4x4_scale_aniso(transformMatrix, transformMatrix, pVS->whl[0] * 0.5f, pVS->whl[1] * 0.5f, 1);
	int vertsN = pMB->vertices.size();
	for (int i = pMB->pCurrentGroup->fromVertexN; i < vertsN; i++) {
		Vertex01* pVX = pMB->vertices.at(i);
		mat4x4_mul_vec4plus(pVX->aPos, transformMatrix, pVX->aPos, 1);
	}
	releaseGroup(pMB);
	return 1;
}
int ModelBuilder::capWrap(ModelBuilder* pMB, VirtualShape* pVS, float angleFrom, float angleTo) {
	// angleFrom/To - in degrees
	lockGroup(pMB);
	//center point
	int n0 = addVertex(pMB, 0, 0, 1, 0, 0, 1);
	float stepZdg = 90.0f / pVS->sections[2]; //in degrees
	float stepRdg = (angleTo - angleFrom) / pVS->sectionsR; //in degrees
	for (int nz = 1; nz <= pVS->sections[2]; nz++) {
		float angleZrd = stepZdg * nz * degrees2radians;
		float kz = cosf(angleZrd);
		float R = sinf(angleZrd);
		for (int rpn = 0; rpn <= pVS->sectionsR; rpn++) {
			// rpn - radial point number
			float angleRd = (angleFrom + stepRdg * rpn) * degrees2radians;
			float kx = cosf(angleRd) * R;
			float ky = sinf(angleRd) * R;
			int nSE = addVertex(pMB, kx, ky, kz, kx, ky, kz);
			if (rpn > 0) {
				if (nz == 1) {
					int nSW = nSE - 1;
					addTriangle(pMB, n0, nSW, nSE);
				}
				else {
					int nSW = nSE - 1;
					int nNW = nSW - pVS->sectionsR - 1;
					int nNE = nSE - pVS->sectionsR - 1;
					add2triangles(pMB, nNW, nNE, nSW, nSE, nz + rpn);
				}
			}
		}
	}
	//scale to desirable diameters
	mat4x4 transformMatrix = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	mat4x4_scale_aniso(transformMatrix, transformMatrix, pVS->whl[0] * 0.5f, pVS->whl[1] * 0.5f, pVS->whl[2]);
	int vertsN = pMB->vertices.size();
	for (int i = pMB->pCurrentGroup->fromVertexN; i < vertsN; i++) {
		Vertex01* pVX = pMB->vertices.at(i);
		mat4x4_mul_vec4plus(pVX->aPos, transformMatrix, pVX->aPos, 1);
	}
	releaseGroup(pMB);
	return 1;
}
int ModelBuilder::groupApplyTexture(ModelBuilder* pMB, std::string applyTo, TexCoords* pTC, TexCoords* pTC2nm) {
	int vertsN = pMB->vertices.size();
	for (int vN = 0; vN < vertsN; vN++) {
		Vertex01* pVX = pMB->vertices.at(vN);
		if(vN < pMB->pCurrentGroup->fromVertexN)
			pVX->flag = -1;
		else
			pVX->flag = 0;
	}
	applyTexture2flagged(&pMB->vertices, applyTo, pTC, false);
	applyTexture2flagged(&pMB->vertices, applyTo, pTC2nm, true);
	return 1;
}
int ModelBuilder::applyTexture2flagged(std::vector<Vertex01*>* pVerts, std::string applyTo, TexCoords* pTC, bool isNormalMap) {
	if (pTC == NULL)
		return 0;
	float posMin[3];
	float posMax[3];
	float posRange[3];
	for (int i = 0; i < 3; i++) {
		posMin[i] = 1000000;
		posMax[i] = -1000000;
	}
	int vertsN = pVerts->size();
	for (int vN = 0; vN < vertsN; vN++) {
		Vertex01* pVX = pVerts->at(vN);
		if (pVX->flag < 0) //ignore
			continue;
		for (int i = 0; i < 3; i++) {
			if (posMin[i] > pVX->aPos[i])
				posMin[i] = pVX->aPos[i];
			if (posMax[i] < pVX->aPos[i])
				posMax[i] = pVX->aPos[i];
		}
	}
	//here we have coordinates range
	for (int i = 0; i < 3; i++)
		posRange[i] = posMax[i] - posMin[i];
	//for "front"
	int xRateIndex = 0;
	bool xRateInverse = false;
	int yRateIndex = 1;
	bool yRateInverse = true;
	if (applyTo.find("front") == 0)
		; //do nothing
	else if (applyTo.find("back") == 0)
		xRateInverse = true;
	else if (applyTo.find("left") == 0)
		xRateIndex = 2;
	else if (applyTo.find("right") == 0) {
		xRateIndex = 2;
		xRateInverse = true;
	}
	else if (applyTo.find("top") == 0) {
		xRateInverse = true;
		yRateIndex = 2;
	}
	else if (applyTo.find("bottom") == 0)
		yRateIndex = 2;

	float xRate = 0;
	float yRate = 0;
	float tuvRange[2];
	tuvRange[0] = pTC->tuvBottomRight[0] - pTC->tuvTopLeft[0];
	tuvRange[1] = pTC->tuvBottomRight[1] - pTC->tuvTopLeft[1];
	for (int vN = 0; vN < vertsN; vN++) {
		Vertex01* pVX = pVerts->at(vN);
		if (pVX->flag < 0) //ignore
			continue;
		if (posRange[xRateIndex] == 0)
			xRate = 0;
		else {
			xRate = (pVX->aPos[xRateIndex] - posMin[xRateIndex]) / posRange[xRateIndex];
			if (xRateInverse)
				xRate = 1.0f - xRate;
		}
		if (posRange[yRateIndex] == 0)
			yRate = 0;
		else {
			yRate = (pVX->aPos[yRateIndex] - posMin[yRateIndex]) / posRange[yRateIndex];
			if (yRateInverse)
				yRate = 1.0f - yRate;
		}
		float* pTuv = pVX->aTuv;
		if(isNormalMap)
			pTuv = pVX->aTuv2;
		pTuv[0] = pTC->tuvTopLeft[0] + tuvRange[0] * xRate;
		pTuv[1] = pTC->tuvTopLeft[1] + tuvRange[1] * yRate;
	}
	return 1;
}
