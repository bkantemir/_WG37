#include "Coords.h"
#include "platform.h"
#include <string>

float PI = 3.141592f;
float degrees2radians = PI / 180.0f;
float radians2degrees = 180.0f / PI;

void Coords::setDegrees(Coords* pC, float ax, float ay, float az) {
	if (pC->eulerDg[0] == ax && pC->eulerDg[1] == ay && pC->eulerDg[2] == az)
		return;
	pC->eulerDg[0] = ax;
	pC->eulerDg[1] = ay;
	pC->eulerDg[2] = az;
	//convert to radians
	pC->eulerRd[0] = pC->eulerDg[0] * degrees2radians;
	pC->eulerRd[1] = pC->eulerDg[1] * degrees2radians;
	pC->eulerRd[2] = pC->eulerDg[2] * degrees2radians;
	//re-build rotation matrix
	eulerRdToMatrix(pC->rotationMatrix, pC->eulerRd);
}
void Coords::eulerRdToMatrix(mat4x4 rotationMatrix, float* eulerRd){
	//builds rotation matrix from Euler angles (in radians)
	mat4x4_identity(rotationMatrix);
	//rotation order: Z-X-Y
	float a = eulerRd[1];
	if (a != 0)
		mat4x4_rotate_Y(rotationMatrix, rotationMatrix, a);
	a = eulerRd[0];
	if (a != 0)
		mat4x4_rotate_X(rotationMatrix, rotationMatrix, a);
	a = eulerRd[2];
	if (a != 0)
		mat4x4_rotate_Z(rotationMatrix, rotationMatrix, a);
}
void Coords::setPosition(Coords* pC, float kx, float ky, float kz) {
	pC->pos[0] = kx;
	pC->pos[1] = ky;
	pC->pos[2] = kz;
}
void Coords::setRotationMatrix(Coords* pC, mat4x4 m) {
	memcpy(pC->rotationMatrix, m, sizeof(pC->rotationMatrix));
	//update Euler angles
	matrixToEulerRd(pC->eulerRd, pC->rotationMatrix);

	pC->eulerDg[0] = pC->eulerRd[0] * radians2degrees;
	pC->eulerDg[1] = pC->eulerRd[1] * radians2degrees;
	pC->eulerDg[2] = pC->eulerRd[2] * radians2degrees;
}
void Coords::matrixToEulerRd(float* eulerRd, mat4x4 m) {
	//calculates Euler angles (in radians) from matrix
	float yaw, pitch, roll;

	if (m[1][2] > 0.998 || m[1][2] < -0.998) { // singularity at south or north pole
		yaw = atan2f(-m[2][0], m[0][0]);
		roll = 0;
	}
	else {
		yaw = atan2f(-m[0][2], m[2][2]);
		roll = atan2f(-m[1][0], m[1][1]);
	}
	pitch = asinf(m[1][2]);

	eulerRd[0] = pitch;
	eulerRd[1] = yaw;
	eulerRd[2] = roll;
}
/*
void mylogMatrix(std::string name, mat4x4 m) {
	mylog("%s:\n", name);
	mylog("%f x %f x %f x %f\n", m[0][0], m[0][1], m[0][2], m[0][3]);
	mylog("%f x %f x %f x %f\n", m[1][0], m[1][1], m[1][2], m[1][3]);
	mylog("%f x %f x %f x %f\n", m[2][0], m[2][1], m[2][2], m[2][3]);
	mylog("%f x %f x %f x %f\n", m[3][0], m[3][1], m[3][2], m[3][3]);
}
void mylogVec3(std::string name, float* v) {
	mylog("%s: %f x %f x %f\n", name, v[0], v[1], v[2]);
}
*/
