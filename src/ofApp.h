#pragma once

#include <fstream>
#include <iostream>
#include <time.h>

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxTrueTypeFontUC.h"
#include "ofxGui.h"
#include "ofxGuiExtended.h"

//#include "ofxObjLoader.h"

#include "ofxNi2.h"
//#include "ofxNiTE2.h"

#define STRINGIFY(A) #A

#define MAX_MODEL_ARRAY 200


class MyGuiItem {
private:
public:
    ofRectangle rect;
    string name;
    string label;
    int type;
    int status;
};



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
     void setPos();
    void dataLoad();
    void saveMapFile();
    void loadMapFile(string meshDataDirPath);
    void drawScaleGrid(float areaSize, int gridSpan);
    void initAppVars();
    void settingAppVarsOnBoot();
    
    ofRectangle getSubRect(ofRectangle parentRect, ofRectangle subRect);
    
    void detailViewNextModel(int mod);
    void myGuiSetup();
    
    ofxTrueTypeFontUC font;
    ofxTrueTypeFontUC fontSmall;
    ofxTrueTypeFontUC fontLarge;
    ofxTrueTypeFontUC fontDebugPrint;
    ofxTrueTypeFontUC fontMyGui;
    
    ofxAssimpModelLoader asModelObj[MAX_MODEL_ARRAY][1000];
    ofMesh modelList[MAX_MODEL_ARRAY][1000];
    int modelFlagList[MAX_MODEL_ARRAY];
    ofImage modelImageList[MAX_MODEL_ARRAY][300];
    int scanTimeRecordList[MAX_MODEL_ARRAY][1000][4];
    time_t scanTimeRecordTimeStructureList[MAX_MODEL_ARRAY][1000];
    long scanUnixTimeLongIntList[MAX_MODEL_ARRAY][1000];
    long scanUnixTimeModelMinList[MAX_MODEL_ARRAY];
    long scanUnixTimeModelMaxList[MAX_MODEL_ARRAY];
    long scanUnixTimeSelectedModelsMin;
    long scanUnixTimeSelectedModelsMax;
    long scanUnixTimeAllItemMin;
    long scanUnixTimeAllItemMax;
    int selectedItemIdOnScene[MAX_MODEL_ARRAY];
    int scanTimeRecordMaxTime[MAX_MODEL_ARRAY];
    double scanGpsDataList[MAX_MODEL_ARRAY][1000][8];
    double scanGpsDataMinLat;
    double scanGpsDataMaxLat;
    double scanGpsDataMinLong;
    double scanGpsDataMaxLong;
    string meshNameList[1024];
    int maxMeshNumList[1024];
    string mapId[1024];
    int mapNum[1024][16];
    float modelHeightList[1024];
    ofPoint modelSceneMin[MAX_MODEL_ARRAY];
    ofPoint modelSceneMax[MAX_MODEL_ARRAY];
    ofPoint modelSceneCenter[MAX_MODEL_ARRAY];
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
    int skipLoadFrame;

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
    ofxPanelExtended gui;
    ofxPanelExtended guiMapEdit;
    ofxPanelExtended guiPlayItem;
    ofxPanelExtended guiPlayControlBar;
    ofxPanelExtended guiPlayControlMenu;
    
    ofxGuiPage guiPage;
    
    ofxTabbedPages guiTabbedPages;
    ofxGuiPage guiPage1, guiPage2;

    
    ofxFloatSlider radius;
    ofxFloatSlider uiThumbnailIconDistance;
    ofxIntSlider uiIconNumX;
    ofxIntSlider uiMeshDrawType;
    ofxIntSlider uiPlayMode;
    ofxIntSlider uiColorMode;
    ofxIntSlider uiFramerate;
    ofxIntSlider uiGpsMapMode;
    ofxVec2Slider position;
    ofxToggle uiBtnPlayRealtime;
    ofxToggle uiBtnLight;
    ofxToggle uiBtnTimerControl;
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
    ofParameter<bool> toggle_param, uiBtnPlayPause;
    ofxMinimalToggle uiBtnPlayPauseParts;
    ofxGuiSpacer uiTimeBarSpacer;
    
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
    bool dispPlayControl;
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
    
    bool dualColorSystem;
    int colorMode = 0;

    ofFbo fboFront;
    ofFbo fboSide;
    ofFbo fboTop;
    ofFbo fboCam;
    
    bool loadPictureMode;
    bool loadVertexColorObj;
    
    ofxNI2::Device *oniDevice;
    ofxNI2::IrStream oniIr;
    ofxNI2::ColorStream oniColor;
    ofxNI2::DepthStream oniDepth;
    
    ofRectangle mainView;
    ofRectangle myGuiMain;
    ofRectangle myGuiMainMenu;
    ofRectangle myGuiMainMenuDiff;
    ofRectangle myGuiMainTimebar;
    ofRectangle myGuiMainTimebarDiff;
    ofRectangle myGuiSub;
    ofRectangle myGuiDetailLeftButton;
    ofRectangle myGuiDetailRightButton;
    ofRectangle myGuiDispGuiToggle;
    ofRectangle myGuiSeekBar;
    
    ofRectangle myGuiPlayButton;
    
    
    ofMatrix4x4 modelMatrixList[MAX_MODEL_ARRAY];
    
    MyGuiItem myGuiItemList[100];
    MyGuiItem myGuiItemDetailLeftButton;
    MyGuiItem myGuiItemDetailRightButton;
    
    
    bool useOpenNi;
    bool dataLoadOnAppBoot;
};

