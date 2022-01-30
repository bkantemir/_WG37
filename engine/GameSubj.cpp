#include "GameSubj.h"
#include "platform.h"

GameSubj::~GameSubj() {
    if (pAltMaterial != NULL)
        delete pAltMaterial;
}
void GameSubj::buildModelMatrix(GameSubj* pGS) {
    mat4x4_translate(pGS->ownModelMatrix, pGS->ownCoords.pos[0], pGS->ownCoords.pos[1], pGS->ownCoords.pos[2]);
    //rotation order: Z-X-Y
    mat4x4_mul(pGS->ownModelMatrix, pGS->ownModelMatrix, *(pGS->ownCoords.getRotationMatrix()));

    if (pGS->scale[0] != 1 || pGS->scale[1] != 1 || pGS->scale[2] != 1)
        mat4x4_scale_aniso(pGS->ownModelMatrix, pGS->ownModelMatrix, pGS->scale[0], pGS->scale[1], pGS->scale[2]);
}
int GameSubj::moveSubj(GameSubj* pGS) {
    if (pGS->ownSpeed.getRd(0) != 0 || pGS->ownSpeed.getRd(1) != 0 || pGS->ownSpeed.getRd(2) != 0) {
        //apply angle speed
        mat4x4 newRotationMatrix;
        mat4x4_mul(newRotationMatrix, *(pGS->ownCoords.getRotationMatrix()), *(pGS->ownSpeed.getRotationMatrix()));
        pGS->ownCoords.setRotationMatrix(newRotationMatrix);
    }
    return 1;
}
