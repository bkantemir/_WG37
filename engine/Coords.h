#pragma once
#include "linmath.h"

class Coords
{
private:
	float eulerDg[3] = { 0,0,0 }; //Euler angles (yaw, pitch, and roll) in degrees
	float eulerRd[3] = { 0,0,0 }; //Euler angles in radians
	mat4x4 rotationMatrix = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	//mat4x4 rotationMatrix = { {1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1} };
public:
	float pos[4] = { 0,0,0,0 }; //x,y,z position + 4-th element for compatibility with 3D 4x4 matrices math
public:
	void setDegrees(float ax, float ay, float az) { setDegrees(this, ax, ay, az); };
	static void setDegrees(Coords* pC, float ax, float ay, float az);
	float getRd(int i) { return eulerRd[i]; }; //get angle in radians
	//float getDg(int i) { return eulerDg[i]; }; //get angle in degrees
	void setPosition(float kx, float ky, float kz) { setPosition(this, kx, ky, kz); };
	static void setPosition(Coords* pC, float kx, float ky, float kz);
	mat4x4* getRotationMatrix() { return &rotationMatrix; };
	void setRotationMatrix(mat4x4 m) { setRotationMatrix(this, m); };
	static void setRotationMatrix(Coords* pC, mat4x4 m);
	static void eulerRdToMatrix(mat4x4 rotationMatrix, float* eulerRd);
	static void matrixToEulerRd(float* eulerRd, mat4x4 m);
};

