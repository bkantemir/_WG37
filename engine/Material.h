#pragma once
#include "MyColor.h"
#include <string>

class Material
{
public:
	char shaderType[20] = "";
	int shaderN = -1;
	int primitiveType = GL_TRIANGLES;
	MyColor uColor;
	int uTex0 = -1;
	int uTex1mask = -1;
	int uTex2nm = -1;
	int uTex3 = -1;
	int uTex1alphaChannelN = 3; //default - alpha channel for mask
	int uTex1alphaNegative = 0; //default - alpha channel not negative
	int uTex0translateChannelN = -1; //translate tex0 to tex3 by channelN. Default -1 - don't translate

	int uAlphaBlending = 0; //for semi-transparency
	float uAlphaFactor = 1; //for semi-transparency
	float uAmbient = 0.4f; //ambient light
	//specular light parameters
	float uSpecularIntencity = 0.8f;
	float uSpecularMinDot = 0.8f;
	float uSpecularPowerOf = 20.0f;

	float lineWidth = 1;
	int zBuffer = 1;
	int zBufferUpdate = 1;
public:
	int pickShaderNumber() { return pickShaderNumber(this); };
	static int pickShaderNumber(Material* pMT);
	void setShaderType(std::string needType) { setShaderType(this, needType); };
	static void setShaderType(Material* pMT, std::string needType) { myStrcpy_s(pMT->shaderType, 20, (char*)needType.c_str()); };
	void clear() { clear(this); };
	static void clear(Material* pMT);
	int assignShader(std::string needType) { return assignShader(this, needType); };
	static int assignShader(Material* pMT, std::string needType);
};
