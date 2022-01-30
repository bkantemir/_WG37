#pragma once
#include "ModelBuilder1base.h"
#include "TexCoords.h"

class ModelBuilder : public ModelBuilder1base
{
public:
	virtual ~ModelBuilder();
	static int buildFace(ModelBuilder* pMB, std::string applyTo, VirtualShape* pVS, TexCoords* pTC = NULL, TexCoords* pTC2nm = NULL);
	static int buildBoxFace(ModelBuilder* pMB, std::string applyTo, VirtualShape* pVS, TexCoords* pTC = NULL, TexCoords* pTC2nm = NULL);
	static int buildBoxFacePlain(ModelBuilder* pMB, std::string applyTo, VirtualShape* pVS);
	static int buildBoxFaceTank(ModelBuilder* pMB, std::string applyTo, VirtualShape* pVS);
	static int cylinderWrap(ModelBuilder* pMB, VirtualShape* pVS, float angleFrom, float angleTo);
	static int capWrap(ModelBuilder* pMB, VirtualShape* pVS, float angleFrom, float angleTo);
	static int groupApplyTexture(ModelBuilder* pMB, std::string applyTo, TexCoords* pTC, TexCoords* pTC2nm = NULL);
	static int applyTexture2flagged(std::vector<Vertex01*>* pVX, std::string applyTo, TexCoords* pTC, bool isNormalMap);
};

