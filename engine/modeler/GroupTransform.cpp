#include "GroupTransform.h"
#include "utils.h"

extern float degrees2radians;

int GroupTransform::executeGroupTransform(ModelBuilder1base* pMB, GroupTransform* pGT) {
	//pMB->moveGroupDg(pMB, pGT->spin[0], pGT->spin[1], pGT->spin[2], pGT->shift[0], pGT->shift[1], pGT->shift[2]);
	flagSelection(pGT, &pMB->vertices, &pMB->triangles);
	transformFlagged(pGT, &pMB->vertices);
	return 1;
}
void GroupTransform::limit2mark(GroupTransform* pGT, std::string mark0) {
	std::string outStr;
	outStr.append("<" + mark0 + ">");
	myStrcpy_s(pGT->mark, 32, outStr.c_str());
}
int GroupTransform::flagSelection(GroupTransform* pGT, std::vector<Vertex01*>* pVertices, std::vector<Triangle01*>* pTriangles) {
	if (pVertices != NULL) {
		bool checkSize = false;
		for (int i = 0; i < 3; i++) {
			if (pGT->pMin[i] > -1000000) {
				checkSize = true;
				break;
			}
			if (pGT->pMax[i] < 1000000) {
				checkSize = true;
				break;
			}
		}
		int totalN = pVertices->size();
		for (int vN = 0; vN < totalN; vN++) {
			Vertex01* pV = pVertices->at(vN);
			if (pGT->pGroup != NULL) {
				if (vN < pGT->pGroup->fromVertexN) {
					pV->flag = -1;
					continue;
				}
			}
			if (strcmp(pGT->mark, "") != 0) {
				if (strstr(pV->marks, pGT->mark) == nullptr) {
					pV->flag = -1;
					continue;
				}
			}
			if (checkSize) {
				bool fits = true;
				for (int i = 0; i < 3; i++) {
					if (pV->aPos[i] < pGT->pMin[i]) {
						fits = false;
						break;
					}
					if (pV->aPos[i] > pGT->pMax[i]) {
						fits = false;
						break;
					}
				}
				if (!fits) {
					pV->flag = -1;
					continue;
				}
			}
			pV->flag = 0;
		}
	}
	if (pTriangles != NULL) {
		int totalN = pTriangles->size();
		for (int tN = 0; tN < totalN; tN++) {
			Triangle01* pT = pTriangles->at(tN);
			if (pGT->pGroup != NULL) {
				if (tN < pGT->pGroup->fromTriangleN) {
					pT->flag = -1;
					continue;
				}
			}
			if (strcmp(pGT->mark, "") != 0) {
				if (strstr(pT->marks, pGT->mark) == nullptr) {
					pT->flag = -1;
					continue;
				}
			}
			pT->flag = 0;
		}
	}
	return 1;
}
int GroupTransform::cloneFlagged(ModelBuilder1base* pMB,
	std::vector<Vertex01*>* pVxDst, std::vector<Triangle01*>* pTrDst,
	std::vector<Vertex01*>* pVxSrc, std::vector<Triangle01*>* pTrSrc) {
	int vertsNsrc = pVxSrc->size();
	int trianglesNsrc = pTrSrc->size();
	int vertsN0dst = pVxDst->size();
	int trianglesN0dst = pTrDst->size();
	for (int i = 0; i < vertsNsrc; i++) {
		Vertex01* pV0 = pVxSrc->at(i);
		if (pV0->flag < 0)
			continue;
		pV0->altN = pVxDst->size();
		Vertex01* pV = new Vertex01(*pV0);
		pVxDst->push_back(pV);
		//overwrite marks
		if (pMB != NULL) {
			myStrcpy_s(pV->marks, 124, pMB->pCurrentGroup->marks);
			pV->subjN = pMB->usingSubjN;
		}
		pV->flag = -1;
	}
	for (int i = 0; i < trianglesNsrc; i++) {
		Triangle01* pT0 = pTrSrc->at(i);
		if (pT0->flag < 0)
			continue;
		Triangle01* pT = new Triangle01(*pT0);
		pTrDst->push_back(pT);
		//overwrite marks
		if (pMB != NULL) {
			myStrcpy_s(pT->marks, 124, pMB->pCurrentGroup->marks);
			pT->subjN = pMB->usingSubjN;
		}
		pT->flag = -1;
	}
	refactorTriangles(pTrDst, trianglesN0dst, pVxSrc);
	return 1;
}
int GroupTransform::refactorTriangles(std::vector<Triangle01*>* pTrDst, int trianglesN0dst, std::vector<Vertex01*>* pVxSrc){
	//re-factor triangles idx, adjusting triangles verts #s
	int trianglesNdst = pTrDst->size();
	for (int tN = trianglesN0dst; tN < trianglesNdst; tN++) {
		Triangle01* pT = pTrDst->at(tN);
		for (int i = 0; i < 3; i++) {
			int vN = pT->idx[i];
			Vertex01* pV = pVxSrc->at(vN);
			pT->idx[i] = pV->altN;
		}
	}
	return 1;
}
int GroupTransform::buildTransformMatrix(GroupTransform* pGT, mat4x4* pTransformMatrix) {
	mat4x4_identity(*pTransformMatrix);
	if (!pGT->onThe.empty()) {
		if (pGT->onThe.compare("back") == 0)
			mat4x4_rotate_Y(*pTransformMatrix, *pTransformMatrix, degrees2radians * 180);
		else if (pGT->onThe.compare("left") == 0)
			mat4x4_rotate_Y(*pTransformMatrix, *pTransformMatrix, -degrees2radians * 90);
		else if (pGT->onThe.compare("right") == 0)
			mat4x4_rotate_Y(*pTransformMatrix, *pTransformMatrix, degrees2radians * 90);
		else if (pGT->onThe.compare("top") == 0) {
			mat4x4_rotate_Y(*pTransformMatrix, *pTransformMatrix, degrees2radians * 180);
			mat4x4_rotate_X(*pTransformMatrix, *pTransformMatrix, -degrees2radians * 90);
		}
		else if (pGT->onThe.compare("bottom") == 0)
			mat4x4_rotate_X(*pTransformMatrix, *pTransformMatrix, degrees2radians * 90);
	}
	if(!v3equals(pGT->shift, 0))
		mat4x4_translate_in_place(*pTransformMatrix, pGT->shift[0], pGT->shift[1], pGT->shift[2]);

	//rotation order: Z-X-Y
	if (pGT->spin[1] != 0)
		mat4x4_rotate_Y(*pTransformMatrix, *pTransformMatrix, degrees2radians * pGT->spin[1]);
	if (pGT->spin[0] != 0)
		mat4x4_rotate_X(*pTransformMatrix, *pTransformMatrix, degrees2radians * pGT->spin[0]);
	if (pGT->spin[2] != 0)
		mat4x4_rotate_Z(*pTransformMatrix, *pTransformMatrix, degrees2radians * pGT->spin[2]);

	if (!v3equals(pGT->scale, 1))
		mat4x4_scale_aniso(*pTransformMatrix, *pTransformMatrix, pGT->scale[0], pGT->scale[1], pGT->scale[2]);
	return 1;
}
int GroupTransform::transformFlagged(GroupTransform* pGT, std::vector<Vertex01*>* pVx) {
	//moves and rotates vertex group
	//rotation angles are set in degrees
	mat4x4 transformMatrix;
	buildTransformMatrix(pGT, &transformMatrix);
	transformFlaggedMx(pVx, &transformMatrix);
	return 1;
}
int GroupTransform::transformFlaggedMx(std::vector<Vertex01*>* pVx, mat4x4* pTransformMatrix) {
	//moves and rotates vertex group
	int vertsN = pVx->size();
	for (int i = 0; i < vertsN; i++) {
		Vertex01* pV = pVx->at(i);
		if (pV->flag < 0)
			continue;
		mat4x4_mul_vec4plus(pV->aPos, *pTransformMatrix, pV->aPos, 1);
		//normal
		mat4x4_mul_vec4plus(pV->aNormal, *pTransformMatrix, pV->aNormal, 0);
		vec3_norm(pV->aNormal, pV->aNormal);
	}
	return 1;
}
void GroupTransform::flagAll(std::vector<Vertex01*>* pVx, std::vector<Triangle01*>* pTr) {
	//set flags
	for (int i = pVx->size() - 1; i >= 0; i--)
		pVx->at(i)->flag = 0;
	for (int i = pTr->size() - 1; i >= 0; i--)
		pTr->at(i)->flag = 0;
}
int GroupTransform::buildBoundingBoxFlagged(float* bbMin, float* bbMax, std::vector<Vertex01*>* pVx) {
	v3copy(bbMin, pVx->at(0)->aPos);
	v3copy(bbMax, pVx->at(0)->aPos);
	//scan all flagged except #0
	for (int vN = pVx->size() - 1; vN > 0; vN--) {
		Vertex01* pV = pVx->at(vN);
		if (pV->flag < 0)
			continue;
		for(int i=0;i<3;i++){
			if (bbMin[i] > pV->aPos[i])
				bbMin[i] = pV->aPos[i];
			if (bbMax[i] < pV->aPos[i])
				bbMax[i] = pV->aPos[i];
		}
	}
	return 1;
}