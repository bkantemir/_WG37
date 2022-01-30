#include "TheGame.h"
#include "platform.h"
#include "utils.h"
#include "linmath.h"
#include "Texture.h"
#include "Shader.h"
#include "DrawJob.h"
#include "ModelBuilder.h"
#include "TexCoords.h"
#include "ModelLoader.h"

extern std::string filesRoot;
extern float degrees2radians;

std::vector<GameSubj*> TheGame::gameSubjs;

int TheGame::getReady() {
    bExitGame = false;
    Shader::loadShaders();
    glEnable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);

    int subjN = ModelLoader::loadModel(&gameSubjs, "/dt/models/misc/marlboro01/root01.txt", "");
    GameSubj* pGS = gameSubjs.at(subjN);
    pGS->name.assign("box1");
    pGS->ownSpeed.setDegrees(1.5, 1, 0.5);
    pGS->ownCoords.setDegrees(0,30, 0);

    //===== set up camera
    mainCamera.ownCoords.setDegrees(15, 180, 0); //set camera angles/orientation
    mainCamera.viewRangeDg = 20;
    mainCamera.stageSize[0] = 80;
    mainCamera.stageSize[1] = 120;
    memcpy(mainCamera.lookAtPoint, pGS->ownCoords.pos, sizeof(float) * 3);
    mainCamera.onScreenResize();

    //===== set up light
    v3set(dirToMainLight, -1, 1, 1);
    vec3_norm(dirToMainLight, dirToMainLight);

    return 1;
}

int TheGame::drawFrame() {
    myPollEvents(); 

    //glClearColor(0.0, 0.0, 0.5, 1.0);
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4x4 mProjection, mViewProjection, mMVP, mMV4x4;
    //mat4x4_ortho(mProjection, -(float)screenSize[0] / 2, (float)screenSize[0] / 2, -(float)screenSize[1] / 2, (float)screenSize[1] / 2, 100.f, 500.f);
    float nearClip = mainCamera.focusDistance - 55;
    float farClip = mainCamera.focusDistance + 55;
    if (nearClip < 0) nearClip = 0;
    mat4x4_perspective(mProjection, mainCamera.viewRangeDg * degrees2radians, screenAspectRatio, nearClip, farClip);
    mat4x4_mul(mViewProjection, mProjection, mainCamera.lookAtMatrix);
    mViewProjection[1][3] = 0; //keystone effect

    //scan subjects
    int subjsN = gameSubjs.size();
    for (int subjN = 0; subjN < subjsN; subjN++) {
        GameSubj* pGS = gameSubjs.at(subjN);
        //behavior - apply rotation speed
        pGS->moveSubj();
        //prepare subject for rendering
        pGS->buildModelMatrix(pGS);
        //build MVP matrix for given subject
        mat4x4_mul(mMVP, mViewProjection, pGS->ownModelMatrix);
        //build Model-View (rotation) matrix for normals
        mat4x4_mul(mMV4x4, mainCamera.lookAtMatrix, (vec4*)pGS->ownCoords.getRotationMatrix());
        //convert to 3x3 matrix
        float mMV3x3[3][3];
        for (int y = 0; y < 3; y++)
            for (int x = 0; x < 3; x++)
                mMV3x3[y][x] = mMV4x4[y][x];
        //subj's distance from camera
        float cameraSpacePos[4];
        mat4x4_mul_vec4plus(cameraSpacePos, mainCamera.lookAtMatrix, pGS->ownCoords.pos, 1);
        float zDistance = abs(cameraSpacePos[2]);
        float cotangentA = 1.0f / tanf(degrees2radians * mainCamera.viewRangeDg / 2.0);
        float halfScreenVertSizeInUnits = zDistance / cotangentA;
        float sizeUnitPixelsSize = screenSize[1] / 2.0 / halfScreenVertSizeInUnits;
        //render subject
        for (int i = 0; i < pGS->djTotalN; i++) {
            DrawJob* pDJ = DrawJob::drawJobs.at(pGS->djStartN + i);
            pDJ->execute((float*)mMVP, *mMV3x3, (float*)pGS->ownModelMatrix, dirToMainLight, mainCamera.ownCoords.pos, sizeUnitPixelsSize, NULL);
        }
    }
    //synchronization
    while (1) {
        long long int currentMillis = getSystemMillis();
        long long int millisSinceLastFrame = currentMillis - lastFrameMillis;
        if (millisSinceLastFrame >= millisPerFrame) {
            lastFrameMillis = currentMillis;
            break;
        }
    }
    mySwapBuffers();
    return 1;
}
int TheGame::cleanUp() {
    int itemsN = gameSubjs.size();
    //delete all UISubjs
    for (int i = 0; i < itemsN; i++) {
        GameSubj* pGS = gameSubjs.at(i);
        delete pGS;
    }
    gameSubjs.clear();
    //clear all other classes
    Texture::cleanUp();
    Shader::cleanUp();
    DrawJob::cleanUp();
    return 1;
}
int TheGame::onScreenResize(int width, int height) {
    if (screenSize[0] == width && screenSize[1] == height)
        return 0;
    screenSize[0] = width;
    screenSize[1] = height;
    screenAspectRatio = (float)width / height;
    glViewport(0, 0, width, height);
    mainCamera.onScreenResize();
    mylog(" screen size %d x %d\n", width, height);
    return 1;
}
GameSubj* TheGame::newGameSubj(std::string subjClass) {
    return (new GameSubj());
}
int TheGame::run() {
    getReady();
    while (!bExitGame) {
        drawFrame();
    }
    cleanUp();
    return 1;
}

