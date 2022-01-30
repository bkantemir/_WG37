#include "MaterialAdjust.h"

int MaterialAdjust::adjust(Material* pMT, MaterialAdjust* pMA) {
	if (pMA->b_shaderType)
		pMT->setShaderType(pMA->shaderType);
	if (pMA->b_primitiveType)
		pMT->primitiveType = pMA->primitiveType;
	if (pMA->b_uColor) {
		memcpy(&pMT->uColor, &pMA->uColor, sizeof(MyColor));
		pMT->uTex0 = -1;
	}
	if (pMA->b_uTex0) {
		pMT->uTex0 = pMA->uTex0;
		pMT->uColor.setUint32(0);
	}
	if (pMA->b_uTex1mask)
		pMT->uTex1mask = pMA->uTex1mask;
	if (pMA->b_uTex2nm)
		pMT->uTex2nm = pMA->uTex2nm;
	if (pMA->b_uTex3)
		pMT->uTex3 = pMA->uTex3;
	if (pMA->b_uTex1alphaChannelN)
		pMT->uTex1alphaChannelN = pMA->uTex1alphaChannelN;
	if (pMA->b_uTex1alphaNegative)
		pMT->uTex1alphaNegative = pMA->uTex1alphaNegative;
	if (pMA->b_uTex0translateChannelN)
		pMT->uTex0translateChannelN = pMA->uTex0translateChannelN;
	if (pMA->b_uAlphaBlending)
		pMT->uAlphaBlending = pMA->uAlphaBlending;
	if (pMA->b_uAlphaFactor)
		pMT->uAlphaFactor = pMA->uAlphaFactor;
	if (pMA->b_uAmbient)
		pMT->uAmbient = pMA->uAmbient;
	if (pMA->b_uSpecularIntencity)
		pMT->uSpecularIntencity = pMA->uSpecularIntencity;
	if (pMA->b_uSpecularMinDot)
		pMT->uSpecularMinDot = pMA->uSpecularMinDot;
	if (pMA->b_uSpecularPowerOf)
		pMT->uSpecularPowerOf = pMA->uSpecularPowerOf;
	return 1;
}
int MaterialAdjust::setWhat2adjust(MaterialAdjust* pMA, std::string tagStr) {
	if (tagStr.find("uTex0") != std::string::npos)
		if(pMA->uTex0 >= 0)
			pMA->b_uTex0 = true;
	if (tagStr.find("uTex1mask") != std::string::npos)
		pMA->b_uTex1mask = true;
	if (tagStr.find("uTex2nm") != std::string::npos)
		pMA->b_uTex2nm = true;
	if (tagStr.find("uTex3") != std::string::npos)
		pMA->b_uTex3 = true;
	if (tagStr.find("mt_type") != std::string::npos)
		pMA->b_shaderType = true;
	if (tagStr.find("uColor") != std::string::npos)
		pMA->b_uColor = true;
	if (tagStr.find("primitiveType") != std::string::npos)
		pMA->b_primitiveType = true;
	if (tagStr.find("uTex1alphaChannelN") != std::string::npos)
		pMA->b_uTex1alphaChannelN = true;
	if (tagStr.find("uTex0translateChannelN") != std::string::npos)
		pMA->b_uTex0translateChannelN = true;
	if (tagStr.find("uAlphaBlending") != std::string::npos)
		pMA->b_uAlphaBlending = true;
	if (tagStr.find("uAlphaFactor") != std::string::npos)
		pMA->b_uAlphaFactor = true;
	if (tagStr.find("uAmbient") != std::string::npos)
		pMA->b_uAmbient = true;
	if (tagStr.find("uSpecularIntencity") != std::string::npos)
		pMA->b_uSpecularIntencity = true;
	if (tagStr.find("uSpecularMinDot") != std::string::npos)
		pMA->b_uSpecularMinDot = true;
	if (tagStr.find("uSpecularPowerOf") != std::string::npos)
		pMA->b_uSpecularPowerOf = true;
	return 1;
}