#pragma once

#include <fstream>
#include <iostream>
#include <time.h>

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxTrueTypeFontUC.h"
#include "ofxGui.h"

#include "ofxObjLoader.h"

#define STRINGIFY(A) #A


class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void resetCam();
    void resetCamDetailView();
    void resetCamListView();
    void gridSwitch();
    void debugInfoSwitch();
    void turnMeshSwitch();
    void setPos();
    void dataLoad();
    void saveMapFile();
    void loadMapFile();
    
    ofxTrueTypeFontUC font;
    ofxTrueTypeFontUC fontSmall;
    ofxTrueTypeFontUC fontLarge;
    
    ofxAssimpModelLoader asModelObj[64][1000];
    ofMesh modelList[64][1000];
    int modelFlagList[64];
    ofImage modelImageList[64][300];
    int scanTimeRecordList[64][1000][4];
    time_t scanTimeRecordTimeStructureList[64][1000];
    long scanUnixTimeLongIntList[64][1000];
    long scanUnixTimeModelMinList[64];
    long scanUnixTimeModelMaxList[64];
    long scanUnixTimeSelectedModelsMin;
    long scanUnixTimeSelectedModelsMax;
    long scanUnixTimeAllItemMin;
    long scanUnixTimeAllItemMax;
    int selectedItemIdOnScene[64];
    int scanTimeRecordMaxTime[64];
    double scanGpsDataList[64][1000][8];
    double scanGpsDataMinLat;
    double scanGpsDataMaxLat;
    double scanGpsDataMinLong;
    double scanGpsDataMaxLong;
    string meshNameList[1024];
    int maxMeshNumList[1024];
    string mapId[1024];
    int mapNum[1024][16];
    float modelHeightList[1024];
    float modelPosXList[1024];
    float modelPosZList[1024];
    vector<string> dataDirNameList;
    vector< vector<uint64_t> > modelFileSizeList;
    vector <uint64_t> oneModelFileSizeList;
    int modelDataNum;
    uint64_t loadFileSizeAll;
    int totalMaxMeshNum;
    int totalScanTimeRecordMaxTime;
    long seekbarAddTime;

    ofMesh mesh;
    ofLight light;
    
    ofEasyCam eCam;
    
    // openframeworksではofShaderオブジェクトを通してシェーダを使う
    ofShader shader;
    
    ofFile ofFileObj;
    ofCamera ofCam;
    
    string meshDataDirPath;
    string defaultMeshDataDirPath;
    
    // GUIのパラメーター
    ofxPanel gui;
    ofxPanel guiMapEdit;
    ofxPanel guiPlayItem;
    ofxFloatSlider radius;
    ofxFloatSlider uiThumbnailIconDistance;
    ofxIntSlider uiIconNumX;
    ofxIntSlider uiMeshDrawType;
    ofxIntSlider uiPlayMode;
    ofxIntSlider uiFramerate;
    ofxColorSlider color;
    ofxVec2Slider position;
    ofxToggle uiBtnPlayPause;
    ofxToggle uiBtnPlayRealtime;
    ofxToggle uiBtnLight;
    ofxButton uiBtnReset;
    ofxButton uiBtnSelectReset;
    ofxButton uiBtnPlaySelectA;
    ofxButton uiBtnPlaySelectB;
    ofxButton uiBtnPlaySelectC;
    ofxButton uiBtnPlaySelectBack;
    ofxToggle uiBtnGrid;
    ofxToggle uiBtnDebugInfo;
    ofxToggle uiBtnTurnMesh;
    ofxToggle uiBtnLoopPlay;
    ofxToggle uiBtnOrtho;
    ofxToggle uiBtnGpsMapMode;
    
    ofxFloatSlider uiEditPosX;
    ofxFloatSlider uiEditPosY;
    ofxFloatSlider uiEditPosZ;
    ofxFloatSlider uiEditRotX;
    ofxFloatSlider uiEditRotY;
    ofxFloatSlider uiEditRotZ;
    ofxFloatSlider uiEditScale;
    ofxIntSlider uiEditModelSelector;
    ofxIntSlider uiEditReadStartFrame;
    ofxIntSlider uiEditReadEndFrame;
    ofxIntSlider uiEditDisplayFlag;
    ofxIntSlider uiTestSlider;
    
    float prevPosX;
    float prevPosY;

    unsigned int frameCount;
    unsigned int playCount;
    
    
    int maxLoadMeshNum;
    int startPlayMeshAnimNum;
    
    
    int totalLoadedModelNum;
    int maxLoadedMeshNumInAllMesh;
    
    int prevSelectModel;
    
    int selectMeshId;
    
    float thumbnailIconDistance;

    int viewerMode;         // Detail / List / Mapping /  MapEdit
    
    string mapFilePath;
    ofFile mapFile;
    ofBuffer mapBuffer;
    
    
    time_t unixTimeOnOfStarted;
    struct tm *unixTimeOnOfStartedTmStruct;

    bool mapFileExists;
    int mapDataColumns;
    
    bool dispGui;
    bool dispDebugInfoFlag;
    bool dispAllUiFlag;
    bool dispPlayControlUiFlag;
    
    int displayTotalVertices;
    int displayTotalFaces;
    
    int appInitStartTime;
    int appInitEndTime;
    int modeldataLoadingStartTime;
    int modeldataLoadingEndTime;
    
    int modeldataFiles;
    int modeldataDatasetNum;
    
    
    int playSeekTime;
    int nowTime;
    
    bool onPlay;
    int playMode;
    
    int prevPlayTime;
    
    uint64_t  playStartDateTime;
    uint64_t  playStartPrevPos;
    
    bool loopPlay;
    
    int nowPlayTime;
    
    bool prevFramePlayState;
    
};

