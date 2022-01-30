#include "ModelLoader.h"
#include "platform.h"
#include "TheGame.h"
#include "DrawJob.h"
#include "Texture.h"
#include "utils.h"
#include "Polygon.h"

extern TheGame theGame;

int ModelLoader::loadModel(std::vector<GameSubj*>* pSubjsVector0, std::string sourceFile, std::string subjClass) {
	//returns element's (Subj) number or -1
	int subjN = pSubjsVector0->size();
	GameSubj* pGS = theGame.newGameSubj(subjClass);
	pSubjsVector0->push_back(pGS);
	//pGS->djStartN = DrawJob::drawJobs.size();
	ModelLoader* pML = new ModelLoader(pSubjsVector0, subjN, NULL, sourceFile);
	processSource(pML);
	delete pML;
	//pGS->djTotalN = DrawJob::drawJobs.size() - pGS->djStartN;
	return subjN;
}

int ModelLoader::setValueFromIntHashMap(int* pInt, std::map<std::string, int> intHashMap, std::string varName, std::string tagStr) {
	if (!varExists(varName, tagStr))
		return 0;
	std::string str0 = getStringValue(varName, tagStr);
	if (intHashMap.find(str0) == intHashMap.end()) {
		mylog("ERROR in ModelLoader::setValueFromIntMap, %s not found, %s\n", varName.c_str(), tagStr.c_str());
		return -1;
	}
	*pInt = intHashMap[getStringValue(varName, tagStr)];
	return 1;
}
int ModelLoader::setTexture(ModelLoader* pML, int* pInt, std::string txName) {
	ModelBuilder* pMB = pML->pModelBuilder;
	bool resetTexture = false;
	std::string varName = txName + "_use";
	if (varExists(varName, pML->currentTag)) {
		if (setValueFromIntHashMap(pInt, pMB->texturesHashMap, varName, pML->currentTag) == 0) {
			mylog("ERROR in ModelLoader::setTexture: texture not in hashMap: %s\n", pML->currentTag.c_str());
			return -1;
		}
		resetTexture = true;
	}
	else{
		varName = txName + "_src";
		if (varExists(varName, pML->currentTag)) {
			std::string txFile = getStringValue(varName, pML->currentTag);
			varName = txName + "_ckey";
			unsigned int intCkey = 0;
			setUintColorValue(&intCkey, varName, pML->currentTag);
			*pInt = Texture::loadTexture(buildFullPath(pML, txFile), intCkey);
			resetTexture = true;
		}
	}
	if(resetTexture)
		return 1;
	return 0; //texture wasn't reset
}
int ModelLoader::setMaterialTextures(ModelLoader* pML, Material* pMT) {
	if (setTexture(pML, &pMT->uTex0, "uTex0") > 0)
		pMT->uColor.clear();
	setTexture(pML, &pMT->uTex1mask, "uTex1mask");
	setTexture(pML, &pMT->uTex2nm, "uTex2nm");
	setTexture(pML, &pMT->uTex3, "uTex3");
	return 1;
}
int ModelLoader::fillProps_mt(Material* pMT, std::string tagStr, ModelLoader* pML) {
	setCharsValue(pMT->shaderType, 20, "mt_type", tagStr);
	setMaterialTextures(pML, pMT);
	//color
	if (varExists("uColor", tagStr)) {
		unsigned int uintColor = 0;
		setUintColorValue(&uintColor, "uColor", tagStr);
		pMT->uColor.setUint32(uintColor);
		pMT->uTex0 = -1;
	}
	//mylog("mt.uTex0=%d, mt.uTex1mask=%d\n", mt.uTex0, mt.uTex1mask);
	if (varExists("primitiveType", tagStr)) {
		std::string str0 = getStringValue("primitiveType", tagStr);
		if (str0.compare("GL_POINTS") == 0) pMT->primitiveType = GL_POINTS;
		else if (str0.compare("GL_LINES") == 0) pMT->primitiveType = GL_LINES;
		else if (str0.compare("GL_LINE_STRIP") == 0) pMT->primitiveType = GL_LINE_STRIP;
		else if (str0.compare("GL_LINE_LOOP") == 0) pMT->primitiveType = GL_LINE_LOOP;
		else if (str0.compare("GL_TRIANGLE_STRIP") == 0) pMT->primitiveType = GL_TRIANGLE_STRIP;
		else if (str0.compare("GL_TRIANGLE_FAN") == 0) pMT->primitiveType = GL_TRIANGLE_FAN;
		else pMT->primitiveType = GL_TRIANGLES;
	}
	setIntValue(&pMT->uTex1alphaChannelN, "uTex1alphaChannelN", tagStr);
	setIntValue(&pMT->uTex0translateChannelN, "uTex0translateChannelN", tagStr);
	setFloatValue(&pMT->uAlphaFactor, "uAlphaFactor", tagStr);
	if (pMT->uAlphaFactor < 1)
		pMT->uAlphaBlending = 1;
	setIntBoolValue(&pMT->uAlphaBlending, "uAlphaBlending", tagStr);
	if (pMT->uAlphaBlending > 0)
		pMT->zBufferUpdate = 0;
	setFloatValue(&pMT->uAmbient, "uAmbient", tagStr);
	setFloatValue(&pMT->uSpecularIntencity, "uSpecularIntencity", tagStr);
	setFloatValue(&pMT->uSpecularMinDot, "uSpecularMinDot", tagStr);
	setFloatValue(&pMT->uSpecularPowerOf, "uSpecularPowerOf", tagStr);

	setFloatValue(&pMT->lineWidth, "lineWidth", tagStr);
	setIntBoolValue(&pMT->zBuffer, "zBuffer", tagStr);
	if (pMT->zBuffer < 1)
		pMT->zBufferUpdate = 0;
	setIntBoolValue(&pMT->zBufferUpdate, "zBufferUpdate", tagStr);

	return 1;
}
int ModelLoader::processTag(ModelLoader* pML) {
	ModelBuilder* pMB = pML->pModelBuilder;
	if (pML->tagName.compare("texture_as") == 0) {
		//saves texture N in texturesMap under given name
		std::string keyName = getStringValue("texture_as", pML->currentTag);
		if (pMB->texturesHashMap.find(keyName) != pMB->texturesHashMap.end())
			return pMB->texturesHashMap[keyName];
		else { //add new
			std::string txFile = getStringValue("src", pML->currentTag);
			unsigned int intCkey = 0;
			setUintColorValue(&intCkey, "ckey", pML->currentTag);
			int txN = Texture::loadTexture(buildFullPath(pML, txFile), intCkey);
			pMB->texturesHashMap[keyName] = txN;
			//mylog("%s=%d\n", keyName.c_str(), pMB->texturesMap[keyName]);
			return txN;
		}
	}
	if (pML->tagName.compare("mt_type") == 0) {
		//sets current material
		ModelBuilder* pMB = pML->pModelBuilder;
		if (!pML->closedTag) {
			//save previous material in stack
			if (pMB->usingMaterialN >= 0)
				pMB->materialsStack.push_back(pMB->usingMaterialN);
		}
		Material mt;
		fillProps_mt(&mt, pML->currentTag, pML);
		pMB->usingMaterialN = pMB->getMaterialN(pMB, &mt);
		return 1;
	}
	if (pML->tagName.compare("/mt_type") == 0) {
		//restore previous material
		if (pMB->materialsStack.size() > 0) {
			pMB->usingMaterialN = pMB->materialsStack.back();
			pMB->materialsStack.pop_back();
		}
		return 1;
	}
	if (pML->tagName.compare("vs") == 0) {
		//sets virtual shape
		ModelBuilder* pMB = pML->pModelBuilder;
		if (pML->closedTag) {
			if (pMB->pCurrentVShape != NULL)
				delete pMB->pCurrentVShape;
		}
		else { //open tag
			//save previous vshape in stack
			if (pMB->pCurrentVShape != NULL)
				pMB->vShapesStack.push_back(pMB->pCurrentVShape);
		}
		pMB->pCurrentVShape = new VirtualShape();
		fillProps_vs(pMB->pCurrentVShape, pML->currentTag);
		return 1;
	}
	if (pML->tagName.compare("/vs") == 0) {
		//restore previous virtual shape
		if (pMB->vShapesStack.size() > 0) {
			if (pMB->pCurrentVShape != NULL)
				delete(pMB->pCurrentVShape);
			pMB->pCurrentVShape = pMB->vShapesStack.back();
			pMB->vShapesStack.pop_back();
		}
		return 1;
	}
	if (pML->tagName.compare("group") == 0) {
		std::string notAllowed[] = { "pxyz","axyz","align","headTo" };
		int notAllowedLn = sizeof(notAllowed) / sizeof(notAllowed[0]);
		for (int i = 0; i < notAllowedLn; i++)
			if (varExists(notAllowed[i], pML->currentTag)) {
				mylog("ERROR in ModelLoader::processTag: use %s in </group>: %s\n", notAllowed[i].c_str(), pML->currentTag.c_str());
				return -1;
			}
		pMB->lockGroup(pMB);
		//mark
		if (varExists("mark", pML->currentTag))
			addMark(pMB->pCurrentGroup->marks, getStringValue("mark", pML->currentTag));
		return 1;
	}
	if (pML->tagName.compare("/group") == 0) {
		GroupTransform gt;
		fillProps_gt(&gt, pMB, pML->currentTag);
		gt.executeGroupTransform(pMB);

		pMB->releaseGroup(pMB);
		return 1;
	}
	if (pML->tagName.compare("a") == 0)
		return processTag_a(pML); //apply 
	if (pML->tagName.compare("clone") == 0)
		return processTag_clone(pML);
	if (pML->tagName.compare("/clone") == 0)
		return processTag_clone(pML);
	if (pML->tagName.compare("do") == 0)
		return processTag_do(pML);
	if (pML->tagName.compare("a2mesh") == 0)
		return processTag_a2mesh(pML);
	if (pML->tagName.compare("mt_adjust") == 0) {
		if (pML->pMaterialAdjust != NULL)
			mylog("ERROR in ModelLoader::processTag %s, pMaterialAdjust is still busy. File: %s\n", pML->currentTag.c_str(), pML->fullPath.c_str());
		pML->pMaterialAdjust = new (MaterialAdjust);
		fillProps_mt(pML->pMaterialAdjust, pML->currentTag, pML);
		pML->pMaterialAdjust->setWhat2adjust(pML->pMaterialAdjust, pML->currentTag);
		return 1;
	}
	if (pML->tagName.compare("/mt_adjust") == 0) {
		if (pML->pMaterialAdjust != NULL) {
			delete pML->pMaterialAdjust;
			pML->pMaterialAdjust = NULL;
		}
		return 1;
	}
	if (pML->tagName.compare("line") == 0) {
		Material mt;
		//save previous material in stack
		if (pMB->usingMaterialN >= 0){
			pMB->materialsStack.push_back(pMB->usingMaterialN);
			memcpy(&mt, pMB->materialsList.at(pMB->usingMaterialN),sizeof(Material));
		}
		mt.primitiveType = GL_LINE_STRIP;
		fillProps_mt(&mt, pML->currentTag, pML);
		pMB->usingMaterialN = pMB->getMaterialN(pMB, &mt);
		//line starts
		pML->lineStartsAt = pMB->vertices.size();
		return 1;
	}
	if (pML->tagName.compare("/line") == 0) {
		pMB->vertices.back()->endOfSequence = 1;
		pML->lineStartsAt = -1;
		//restore previous material
		if (pMB->materialsStack.size() > 0) {
			pMB->usingMaterialN = pMB->materialsStack.back();
			pMB->materialsStack.pop_back();
		}
		return 1;
	}
	if (pML->tagName.compare("p") == 0) {
		//line point
		Vertex01* pV = new Vertex01();
		if (pMB->vertices.size() > pML->lineStartsAt)
			memcpy(pV, pMB->vertices.back(), sizeof(Vertex01));
		pV->subjN = pMB->usingSubjN;
		pV->materialN = pMB->usingMaterialN;
		setFloatArray(pV->aPos, 3, "pxyz", pML->currentTag);
		setFloatValue(&pV->aPos[0], "px", pML->currentTag);
		setFloatValue(&pV->aPos[1], "py", pML->currentTag);
		setFloatValue(&pV->aPos[2], "pz", pML->currentTag);
		float dPos[3] = { 0,0,0 };
		setFloatArray(dPos, 3, "dxyz", pML->currentTag);
		setFloatValue(&dPos[0], "dx", pML->currentTag);
		setFloatValue(&dPos[1], "dy", pML->currentTag);
		setFloatValue(&dPos[2], "dz", pML->currentTag);
		if (!v3equals(dPos, 0))
			for (int i = 0; i < 3; i++)
				pV->aPos[i] += dPos[i];
		pMB->vertices.push_back(pV);
		return 1;
	}
	mylog("ERROR in ModelLoader::processTag, unhandled tag %s, file %s\n", pML->currentTag.c_str(), pML->fullPath.c_str());
	return -1;
}

int ModelLoader::fillProps_vs(VirtualShape* pVS, std::string tagStr) {
	//sets virtual shape
	setCharsValue(pVS->shapeType, 20, "vs", tagStr);
	setFloatArray(pVS->whl, 3, "whl", tagStr);
	//extensions
	float ext;
	if (varExists("ext", tagStr)) {
		setFloatValue(&ext, "ext", tagStr);
		pVS->setExt(ext);
	}
	if (varExists("extX", tagStr)) {
		setFloatValue(&ext, "extX", tagStr);
		pVS->setExtX(ext);
	}
	if (varExists("extY", tagStr)) {
		setFloatValue(&ext, "extY", tagStr);
		pVS->setExtY(ext);
	}
	if (varExists("extZ", tagStr)) {
		setFloatValue(&ext, "extZ", tagStr);
		pVS->setExtZ(ext);
	}
	setFloatValue(&pVS->extU, "extU", tagStr);
	setFloatValue(&pVS->extD, "extD", tagStr);
	setFloatValue(&pVS->extL, "extL", tagStr);
	setFloatValue(&pVS->extR, "extR", tagStr);
	setFloatValue(&pVS->extF, "extF", tagStr);
	setFloatValue(&pVS->extB, "extB", tagStr);
	//sections
	setIntValue(&pVS->sectionsR, "sectR", tagStr);
	setIntValue(&pVS->sections[0], "sectX", tagStr);
	setIntValue(&pVS->sections[1], "sectY", tagStr);
	setIntValue(&pVS->sections[2], "sectZ", tagStr);

	//mylog("pVS->shapeType=%s whl=%fx%fx%f\n", pVS->shapeType, pVS->whl[0], pVS->whl[1], pVS->whl[2]);
	return 1;
}
int ModelLoader::processTag_a(ModelLoader* pML) {
	//apply
	ModelBuilder* pMB = pML->pModelBuilder;
	std::string tagStr = pML->currentTag;
	pMB->lockGroup(pMB);
	//mark
	if (varExists("mark", tagStr))
		addMark(pMB->pCurrentGroup->marks, getStringValue("mark", tagStr));

	std::vector<std::string> applyTosVector = splitString(pML->getStringValue("a", tagStr), ",");
	Material* pMT = pMB->materialsList.at(pMB->usingMaterialN);
	int texN = pMT->uTex1mask;
	if (texN < 0)
		texN = pMT->uTex0;
	float xywh[4] = { 0,0,1,1 };
	TexCoords* pTC = NULL;
	if (varExists("xywh", tagStr)) {
		setFloatArray(xywh, 4, "xywh", tagStr);
		std::string flipStr = getStringValue("flip", tagStr);
		TexCoords tc;
		tc.set(texN, xywh[0], xywh[1], xywh[2], xywh[3], flipStr);
		pTC = &tc;
	}
	TexCoords* pTC2nm = NULL;
	if (varExists("xywh2nm", tagStr)) {
		setFloatArray(xywh, 4, "xywh2nm", tagStr);
		std::string flipStr = getStringValue("flip2nm", tagStr);
		TexCoords tc2nm;
		tc2nm.set(pMT->uTex2nm, xywh[0], xywh[1], xywh[2], xywh[3], flipStr);
		pTC2nm = &tc2nm;
	}
	//adjusted VirtualShape
	VirtualShape* pVS_a = new VirtualShape(*pMB->pCurrentVShape);
	fillProps_vs(pVS_a, tagStr);

	for (int aN = 0; aN < (int)applyTosVector.size(); aN++) {
		pMB->buildFace(pMB, applyTosVector.at(aN), pVS_a, pTC, pTC2nm);
	}
	delete pVS_a;
	//mylog("vertsN=%d\n",pMB->vertices.size());

	GroupTransform GT_a;
	fillProps_gt(&GT_a, pMB, tagStr);
	GT_a.executeGroupTransform(pMB);

	pMB->releaseGroup(pMB);
	return 1;
}
int ModelLoader::processTag_clone(ModelLoader* pML) {
	ModelBuilder* pMB = pML->pModelBuilder;
	if (pML->tagName.compare("clone") == 0) {
		//mark what to clone
		GroupTransform gt;
		gt.pGroup = pMB->pLastClosedGroup;
		gt.flagSelection(&gt, &pMB->vertices, &pMB->triangles);

		//cloning
		pMB->lockGroup(pMB);
		gt.cloneFlagged(pMB, &pMB->vertices, &pMB->triangles, &pMB->vertices, &pMB->triangles);
	}
	GroupTransform gt;
	fillProps_gt(&gt, pMB, pML->currentTag);
	gt.executeGroupTransform(pMB);

	if (pML->tagName.compare("/clone") == 0 || pML->closedTag) {
		pMB->releaseGroup(pMB);
	}
	return 1;
}
int ModelLoader::addMark(char* marks, std::string newMark) {
	if (newMark.empty())
		return 0;
	std::string allMarks;
	allMarks.assign(marks);
	allMarks.append("<" + newMark + ">");
	myStrcpy_s(marks, 124, allMarks.c_str());
	return 1;
}
int ModelLoader::fillProps_gt(GroupTransform* pGT, ModelBuilder* pMB, std::string tagStr) {
	pGT->pGroup = pMB->pCurrentGroup;
	//position
	setFloatArray(pGT->shift, 3, "pxyz", tagStr);
	setFloatValue(&pGT->shift[0], "px", tagStr);
	setFloatValue(&pGT->shift[1], "py", tagStr);
	setFloatValue(&pGT->shift[2], "pz", tagStr);
	//angles
	setFloatArray(pGT->spin, 3, "axyz", tagStr);
	setFloatValue(&pGT->spin[0], "ax", tagStr);
	setFloatValue(&pGT->spin[1], "ay", tagStr);
	setFloatValue(&pGT->spin[2], "az", tagStr);
	//scale
	setFloatArray(pGT->scale, 3, "scale", tagStr);

	pGT->onThe = getStringValue("onThe", tagStr);
	pGT->allign = getStringValue("allign", tagStr);
	pGT->headZto = getStringValue("headZto", tagStr);
	//limit to
	if (varExists("all", tagStr))
		pGT->pGroup = NULL;
	if (varExists("lastClosedGroup", tagStr))
		pGT->pGroup = pMB->pLastClosedGroup;
	if (varExists("markedAs", tagStr))
		pGT->limit2mark(pGT, getStringValue("markedAs", tagStr));
	setFloatArray(pGT->pMin, 3, "xyzMin", tagStr);
	setFloatArray(pGT->pMax, 3, "xyzMax", tagStr);

	if (varExists("sizeD", tagStr)) { //re-size
		float sizeD[3];
		setFloatArray(sizeD, 3, "sizeD", tagStr);
		//bounding box
		pGT->flagSelection(pGT, &pMB->vertices, NULL);
		float bbMin[3];
		float bbMax[3];
		pGT->buildBoundingBoxFlagged(bbMin, bbMax, &pMB->vertices);
		for (int i = 0; i < 3; i++) {
			float size = bbMax[i] - bbMin[i];
			pGT->scale[i] = (size + sizeD[i]) / size;
		}
	}
	return 1;
}
int ModelLoader::processTag_do(ModelLoader* pML) {
	ModelBuilder* pMB = pML->pModelBuilder;
	GroupTransform gt;
	fillProps_gt(&gt, pMB, pML->currentTag);
	gt.flagSelection(&gt, &pMB->vertices, &pMB->triangles);
	gt.transformFlagged(&gt, &pMB->vertices);
	return 1;
}
int ModelLoader::processTag_a2mesh(ModelLoader* pML) {
	ModelBuilder* pMB = pML->pModelBuilder;
	std::string tagStr = pML->currentTag;
	GroupTransform gt;
	fillProps_gt(&gt, pMB, pML->currentTag);
	gt.flagSelection(&gt, &pMB->vertices, &pMB->triangles);
	//clone a copy
	std::vector<Vertex01*> vx1;
	std::vector<Triangle01*> tr1;
	gt.cloneFlagged(NULL, &vx1, &tr1, &pMB->vertices, &pMB->triangles);
	// build transform and inverted martrices
	mat4x4 transformMatrix;
	gt.buildTransformMatrix(&gt, &transformMatrix);
	mat4x4 transformMatrixInverted;
	mat4x4_invert(transformMatrixInverted, transformMatrix);
	//move/rotate cloned
	gt.flagAll(&vx1, &tr1);
	//gt.transformFlagged(&pMB->vertices, &transformMatrixInverted);
	gt.transformFlaggedMx(&vx1, &transformMatrixInverted);

	//gt.cloneFlagged(pMB, &pMB->vertices, &pMB->triangles, &vx1, &tr1);

	float wh[2];
	setFloatArray(wh, 2, "wh", tagStr);
	Polygon frame;
	frame.setRectangle(&frame, wh[0], wh[1]);
	//destination arrays
	std::vector<Vertex01*> vx2;
	std::vector<Triangle01*> tr2;
	Polygon triangle;
	for (int i = tr1.size() - 1; i >= 0; i--) {
		triangle.setTriangle(&triangle, tr1.at(i), &vx1);
		Polygon intersection;
		int pointsN = Polygon::xyIntersection(&intersection, &frame, &triangle);
		if (pointsN > 2) {
			Polygon::buildTriangles(&intersection);
			GroupTransform::flagAll(&intersection.vertices, &intersection.triangles);
			GroupTransform::cloneFlagged(NULL, &vx2, &tr2, &intersection.vertices, &intersection.triangles);
		}
	}
	gt.flagAll(&vx2, &tr2);
	//at this point we have cutted fragment facing us
	int vxTotal = vx2.size();
	int trTotal = tr2.size();
	//apply adjusted material ?
	if (pML->pMaterialAdjust != NULL) {
		//scan vertices to find new (unupdated) material
		int materialNsrc = -1; //which N to replace
		int materialNdst = -1; //replace by N 
		for (int vN = 0; vN < vxTotal; vN++) {
			Vertex01* pV = vx2.at(vN);
			if (pV->flag < 0)
				continue;
			if (materialNsrc == pV->materialN)
				continue;
			//have new material
			materialNsrc = pV->materialN;
			Material mt;
			Material* pMt0 = pMB->materialsList.at(materialNsrc);
			memcpy(&mt, pMt0, sizeof(Material));
			//modify material
			MaterialAdjust::adjust(&mt, pML->pMaterialAdjust);
			materialNdst = pMB->getMaterialN(pMB, &mt);
			if (materialNsrc != materialNdst) {
				//replace mtN in vx and tr arrays
				for (int vN2 = vN; vN2 < vxTotal; vN2++) {
					Vertex01* pV2 = vx2.at(vN2);
					if (pV2->flag < 0)
						continue;
					if (materialNsrc == pV2->materialN)
						pV2->materialN = materialNdst;
				}
				for (int tN2 = 0; tN2 < trTotal; tN2++) {
					Triangle01* pT2 = tr2.at(tN2);
					if (pT2->flag < 0)
						continue;
					if (materialNsrc == pT2->materialN)
						pT2->materialN = materialNdst;
				}
				materialNsrc = materialNdst;
			}
		}
	}
	else { // pML->pMaterialAdjust == NULL, use pMB->usingMaterialN
		for (int vN2 = 0; vN2 < vxTotal; vN2++) {
			Vertex01* pV2 = vx2.at(vN2);
			if (pV2->flag < 0)
				continue;
			pV2->materialN = pMB->usingMaterialN;
		}
		for (int tN2 = 0; tN2 < trTotal; tN2++) {
			Triangle01* pT2 = tr2.at(tN2);
			if (pT2->flag < 0)
				continue;
			pT2->materialN = pMB->usingMaterialN;
		}
	}
	//apply xywh/2nm ?
	if (varExists("xywh", tagStr) || varExists("xywh2nm", tagStr)) {
		Material* pMT = pMB->materialsList.at(vx2.at(0)->materialN);
		float xywh[4] = { 0,0,1,1 };
		TexCoords* pTC = NULL;
		if (varExists("xywh", tagStr)) {
			setFloatArray(xywh, 4, "xywh", tagStr);
			std::string flipStr = getStringValue("flip", tagStr);
			int texN = pMT->uTex1mask;
			if (texN < 0)
				texN = pMT->uTex0;
			TexCoords tc;
			tc.set(texN, xywh[0], xywh[1], xywh[2], xywh[3], flipStr);
			pTC = &tc;
		}
		TexCoords* pTC2nm = NULL;
		if (varExists("xywh2nm", tagStr)) {
			setFloatArray(xywh, 4, "xywh2nm", tagStr);
			std::string flipStr = getStringValue("flip2nm", tagStr);
			TexCoords tc2nm;
			tc2nm.set(pMT->uTex2nm, xywh[0], xywh[1], xywh[2], xywh[3], flipStr);
			pTC2nm = &tc2nm;
		}
		pMB->applyTexture2flagged(&vx2, "front", pTC, false);
		pMB->applyTexture2flagged(&vx2, "front", pTC2nm, true);
	}

	float detachBy =0;
	setFloatValue(&detachBy, "detachBy", tagStr);
	if (detachBy != 0) {
		mat4x4 mx;
		mat4x4_translate(mx, 0, 0, detachBy);
		gt.transformFlaggedMx(&vx2, &mx);
	}
	//move/rotate back
	gt.transformFlaggedMx(&vx2, &transformMatrix);
	//clone back to modelBuilder arrays
	gt.cloneFlagged(pMB, &pMB->vertices, &pMB->triangles, &vx2, &tr2);

	//clear memory
	for (int i = vx1.size() - 1; i >= 0; i--)
		delete vx1.at(i);
	vx1.clear();
	for (int i = tr1.size() - 1; i >= 0; i--)
		delete tr1.at(i);
	tr1.clear();
	for (int i = vx2.size() - 1; i >= 0; i--)
		delete vx2.at(i);
	vx2.clear();
	for (int i = tr2.size() - 1; i >= 0; i--)
		delete tr2.at(i);
	tr2.clear();

	return 1;
}

