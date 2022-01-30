#pragma once
#include "Material.h"

class MaterialAdjust : public Material
{
public:
	bool b_shaderType = false;
	bool b_primitiveType = false;
	bool b_uColor = false;
	bool b_uTex0 = false;
	bool b_uTex1mask = false;
	bool b_uTex2nm = false;
	bool b_uTex3 = false;
	bool b_uTex1alphaChannelN = false;
	bool b_uTex1alphaNegative = false;
	bool b_uTex0translateChannelN = false;

	bool b_uAlphaBlending = false;
	bool b_uAlphaFactor = false;
	bool b_uAmbient = false;
	//specular light parameters
	bool b_uSpecularIntencity = false;
	bool b_uSpecularMinDot = false;
	bool b_uSpecularPowerOf = false;
public:
	static int adjust(Material* pMT, MaterialAdjust* pMA);

	static int setWhat2adjust(MaterialAdjust* pMA, std::string tagStr);
};