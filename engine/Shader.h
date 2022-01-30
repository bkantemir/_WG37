#pragma once
#include "platform.h"
#include <string>
#include <vector>

class Shader
{
public:
    //Shader program's individual descriptor:
    unsigned int GLid = -1; // GL shader id
    char shaderType[20] = "";
    //common variables, "l_" for "location"
    //attributes
    int l_aPos; //attribute position (3D coordinates)
    int l_aTuv; //attribute TUV (texture coordinates)
    int l_aTuv2; //attribute TUV (texture coordinates for normal map)
    int l_aNormal; //attribute normal (3D vector)
    int l_aTangent; //for normal map
    int l_aBinormal; //for normal map
    //uniforms
    int l_uMVP; // transform matrix (Model-View-Projection)
    int l_uMV3x3; // Model-View matrix for normals
    int l_uMM; // Model matrix for HalfVector
    int l_uVectorToLight; //required for light
    int l_uCameraPosition; //required for specular light
    //material's properties
    int l_uColor;
    int l_uTex0; //texture id
    int l_uTex1mask; //transparency map
    int l_uTex2nm; //normal map
    int l_uTex3; //texture id
    int l_uTex1alphaChannelN; //alpha channel for mask
    int l_uTex1alphaNegative; //alpha channel negative
    int l_uTex0translateChannelN; //translate tex0 to tex3 by channelN.
    int l_uAlphaFactor; //for semi-transparency
    int l_uAlphaBlending; //for semi-transparency
    //light:
    int l_uAmbient; //ambient light
    //specular light parameters
    int l_uSpecularIntencity;
    int l_uSpecularMinDot;
    int l_uSpecularPowerOf;
    //end of descriptor

    //static array (vector) of all loaded shaders
    static std::vector<Shader*> shaders;

public:
    static int loadShaders();
    static int cleanUp();
    static unsigned int getGLid(int shN) { return shaders.at(shN)->GLid; };
    static int shaderErrorCheck(int shaderId, std::string ref);
    static int programErrorCheck(int programId, std::string ref);
    static int fillLocations(Shader* pSh);

    static int buildShaderObjectFromFiles(std::string filePathVertexS, std::string filePathFragmentS);
    static int linkShaderProgramFromFiles(const char* filePathVertexS, const char* filePathFragmentS);
	static int compileShaderFromFile(const char* filePath, GLenum shaderType);

    static int buildShaderObjectWithDefines(std::string shaderType, std::string definesString, char* sourceVertex, char* sourceFragment);
    static int linkShaderProgramWithDefines(std::string definesString, char* sourceVertex, char* sourceFragment);
    static int compileShaderWithDefines(std::string definesString, char* shaderSource, GLenum shaderType);

    static int loadShadersGroup(std::string shaderType, std::string optionsString, char* sourceVertex, char* sourceFragment);
};
