#pragma once

#include <fstream>
#include <iostream>
#include <time.h>

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxTrueTypeFontUC.h"
#include "ofxGui.h"
#include "ofxGuiExtended.h"

#include "ofxNi2.h"
//#include "ofxNiTE2.h"

#define STRINGIFY(A) #A

#define MAX_SCENE_ARRAY 10
#define MAX_MODEL_ARRAY 10
#define MAX_MESH_ARRAY 1000


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
    
    // my function ------------------
    
    void resetCam();
    void resetCamDetailView();
    void resetCamListView();
    void gridSwitch();
    void debugInfoSwitch();
    void setPos();
    
    void dataLoad();
    void dataLoadAll();
    void saveMapFile(int index);
    void loadMapFile(string meshDataDirPath, int index);
    void saveMapFileAll();
    void loadMapFileAll(string targetDirPath);
    
    void initAppVars();
    void settingAppVarsOnBoot();
    void setupOfxGui();
    void setupOpenNi();

    void drawUi();
    void drawScaleGrid(float areaSize, int gridSpan);
    void drawOpenNi();
    void drawDetailView(int i, int playFrameSelector);
    void drawListView(int i, int playFrameSelector);
    void drawListViewNormal(int i, int playFrameSelector);
    void drawListViewGpsMap(int i, int playFrameSelector);
    void drawListViewGpsMapWalkThru(int i, int playFrameSelector);
    void drawListViewTrackingMap(int i, int playFrameSelector);
    void drawMapView(int i, int playFrameSelector);
    void updateSeekBar(int x, int y);
    void drawSeekBar();
    void drawDebugInfo();
    void drawCalendarClock(int x, int y);
    void drawViewerModeChanger();
    void drawModelLRSelector();
    void drawPlayControlMenu();
    void detailViewNextModel(int mod);
    void detailViewNextScene(int mod);
    void myGuiSetup();
    vector<string> makeDirNameListTargetDir(string dirPath);
    int countMeshFileNumTargetDir(string dirPath);
    void loadScanTimeRecordFile(string dirPath, int modelIndex);
    void loadMeshDataTargetDir(string dirPath, int modelIndex);
    void drawArrow(ofPoint posA, ofPoint posB, double arrowDeltaLineLength=0);
    
    void loadScenesFile(string scenesFilePath);
    void setDemoStateByScene();
    
    ofRectangle getSubRect(ofRectangle parentRect, ofRectangle subRect);
    
    // Array --------
    
    ofxAssimpModelLoader asModelObj[MAX_SCENE_ARRAY][MAX_MODEL_ARRAY][MAX_MESH_ARRAY];
    //ofMesh modelList[MAX_MODEL_ARRAY][MAX_MESH_ARRAY];
    int modelFlagList[MAX_SCENE_ARRAY][MAX_MODEL_ARRAY];
    ofImage modelImageList[MAX_SCENE_ARRAY][MAX_MODEL_ARRAY][MAX_MESH_ARRAY];
    int scanTimeRecordList[MAX_SCENE_ARRAY][MAX_MODEL_ARRAY][MAX_MESH_ARRAY][4];
    time_t scanTimeRecordTimeStructureList[MAX_SCENE_ARRAY][MAX_MODEL_ARRAY][MAX_MESH_ARRAY];
    long scanUnixTimeLongIntList[MAX_SCENE_ARRAY][MAX_MODEL_ARRAY][MAX_MESH_ARRAY];
    long scanUnixTimeModelMinList[MAX_SCENE_ARRAY][MAX_MODEL_ARRAY];
    long scanUnixTimeModelMaxList[MAX_SCENE_ARRAY][MAX_MODEL_ARRAY];
    long scanUnixTimeSelectedModelsMin[MAX_SCENE_ARRAY];
    long scanUnixTimeSelectedModelsMax[MAX_SCENE_ARRAY];
    long scanUnixTimeAllItemMin[MAX_SCENE_ARRAY];
    long scanUnixTimeAllItemMax[MAX_SCENE_ARRAY];
    int selectedItemIdOnScene[MAX_SCENE_ARRAY][MAX_MODEL_ARRAY];
    int scanTimeRecordMaxTime[MAX_SCENE_ARRAY][MAX_MODEL_ARRAY];
    double scanGpsDataList[MAX_SCENE_ARRAY][MAX_MODEL_ARRAY][MAX_MESH_ARRAY][8];
    int playFrameSelectorList[MAX_SCENE_ARRAY][MAX_MODEL_ARRAY];
    double scanGpsDataMinLat[MAX_SCENE_ARRAY];
    double scanGpsDataMaxLat[MAX_SCENE_ARRAY];
    double scanGpsDataMinLong[MAX_SCENE_ARRAY];
    double scanGpsDataMaxLong[MAX_SCENE_ARRAY];
    string meshNameList[MAX_SCENE_ARRAY][MAX_MESH_ARRAY];
    int maxMeshNumList[MAX_SCENE_ARRAY][MAX_MESH_ARRAY];
    unsigned int meshVertexNumList[MAX_SCENE_ARRAY][MAX_MODEL_ARRAY][MAX_MESH_ARRAY];
    string mapId[MAX_SCENE_ARRAY][MAX_MESH_ARRAY];
    float mapNum[MAX_SCENE_ARRAY][MAX_MESH_ARRAY][40];
    string mapStr[MAX_SCENE_ARRAY][MAX_MESH_ARRAY][10];
    string scenesStr[MAX_SCENE_ARRAY][100];
    
    float modelHeightList[MAX_SCENE_ARRAY][MAX_MESH_ARRAY];
    ofPoint modelSceneMin[MAX_SCENE_ARRAY][MAX_MODEL_ARRAY];
    ofPoint modelSceneMax[MAX_SCENE_ARRAY][MAX_MODEL_ARRAY];
    ofPoint modelSceneCenter[MAX_SCENE_ARRAY][MAX_MODEL_ARRAY];
    float modelPosXList[MAX_SCENE_ARRAY][MAX_MESH_ARRAY];
    float modelPosZList[MAX_SCENE_ARRAY][MAX_MESH_ARRAY];
    vector<string> modelDirNameList[MAX_SCENE_ARRAY];
    vector<string> sceneDirNameList;
    vector< vector<uint64_t> > modelFileSizeList[MAX_SCENE_ARRAY];
    vector <uint64_t> oneModelFileSizeList[MAX_SCENE_ARRAY];
    
    // standard vars --------
    int userSelSceneId;
    int selSceneId;
    int sceneDataNum;
    int modelDataNum[MAX_SCENE_ARRAY];
    int totalMaxMeshNum[MAX_SCENE_ARRAY];
    int totalScanTimeRecordMaxTime[MAX_SCENE_ARRAY];
    int skipLoadFrame;
    int maxLoadMeshNum;
    int startPlayMeshAnimNum;
    int totalLoadedModelNum;
    int maxLoadedMeshNumInAllMesh;
    int prevSelectModel;
    int prevSelectScene;
    int selectMeshId;
    int displayTotalVertices;
    int displayTotalFaces;
    int appInitStartTime;
    int appInitEndTime;
    int modeldataLoadingStartTime;
    int modeldataLoadingEndTime;
    int modeldataFiles;
    int playSeekTime;
    int nowTime;
    int viewerMode;         // Detail / List / Mapping /  MapEdit
    int mapDataColumns;
    int mapStringColumns;
    int playMode;
    int prevPlayTime;
    int nowPlayTime;
    int colorMode;
    int playFrameSelector;
    unsigned int frameCount;
    unsigned int playCount;
    long seekbarAddTime;
    long virtualPlayUnixTime;
    uint64_t loadFileSizeAll;
    uint64_t  playStartDateTime;
    uint64_t  playStartPrevPos;
    float prevPosX;
    float prevPosY;
    float thumbnailIconDistance;
    bool mapFileExists;
    bool dispGui;
    bool dispPlayControl;
    bool dispDebugInfoFlag;
    bool dispAllUiFlag;
    bool dispPlayControlUiFlag;
    bool onPlay;
    bool loopPlay;
    bool prevFramePlayState;
    bool dualColorSystem;
    bool loadPictureMode;
    bool loadVertexColorObj;
    bool useOpenNi;
    bool dataLoadOnAppBoot;
    string meshDataDirPath;
    string defaultMeshDataDirPath;
    string loadModelDirName;
    string mapFilePath[MAX_SCENE_ARRAY];
    string targetDirPath;
    string defaultTargetDirPath;
    time_t unixTimeOnOfStarted;
    struct tm *unixTimeOnOfStartedTmStruct;

    // My Class --------------------
    MyGuiItem myGuiItemList[100];
    MyGuiItem myGuiItemDetailLeftButton;
    MyGuiItem myGuiItemDetailRightButton;
    
    // oF Obj ------------------
    
    ofMesh mesh;
    ofLight light;
    ofEasyCam eCam;
    ofShader shader;    // openframeworksではofShaderオブジェクトを通してシェーダを使う
    ofFile ofFileObj;
    ofCamera ofCam;
    ofFbo fboFront;
    ofFbo fboSide;
    ofFbo fboTop;
    ofFbo fboCam;
    
    ofxTrueTypeFontUC font;
    ofxTrueTypeFontUC fontSmall;
    ofxTrueTypeFontUC fontLarge;
    ofxTrueTypeFontUC fontDebugPrint;
    ofxTrueTypeFontUC fontMyGui;
    
    ofFile mapFile;
    ofBuffer mapBuffer;
    
    ofMatrix4x4 modelMatrixList[MAX_MODEL_ARRAY][MAX_MODEL_ARRAY][MAX_MESH_ARRAY];
    
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
    
    ofQuaternion defaultCamOrientation;
    ofQuaternion defaultCamOrientationGlobal;
    

    // oF addon obj ---------------
    
    // GUIのパラメーター
    ofxPanelExtended gui;
    ofxPanelExtended guiMapEdit;
    ofxPanelExtended guiPlayItem;
    ofxPanelExtended guiPlayControlBar;
    ofxPanelExtended guiPlayControlMenu;
    ofxPanelExtended guiPlayControlMenu2;
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
    // ofParameter
    ofxIntSlider uiModelTransparentParts;
    ofxFloatSlider uiTraceCamHeightParts;
    ofxFloatSlider uiChronoHoloMoveXParts;
    ofxFloatSlider uiChronoHoloMoveYParts;
    ofxLabel uiLabelModelSelectNameParts;

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
    ofParameter<bool> toggle_param;
    ofParameter<bool> uiBtnPlayPause;
    ofParameter<bool> uiBtnDispWindow;
    ofParameter<bool> uiBtnTraceCam;
    ofParameter<bool> uiBtnBgColor;
    ofParameter<bool> uiBtnRootDisplay;
    ofParameter<bool> uiBtnDetailNoMix;
    ofParameter<bool> uiBtnDemoPlay;
    ofParameter<bool> uiBtnChronoAnim;
    ofParameter<int> uiModelTransparent;
    ofParameter<int> uiChronoFrameSkip;
    ofParameter<float> uiChronoHoloMoveX;
    ofParameter<float> uiChronoHoloMoveY;
    ofParameter<float> uiTraceCamHeight;
    ofParameter<string> uiLabelModelSelectName;
    ofxMinimalToggle uiBtnPlayPauseParts;
    ofxMinimalToggle uiBtnDispWindowParts;
    ofxMinimalToggle uiBtnTraceCamParts;
    ofxMinimalToggle uiBtnBgColorParts;
    ofxMinimalToggle uiBtnRootDisplayParts;
    ofxMinimalToggle uiBtnDetailNoMixParts;
    ofxMinimalToggle uiBtnDemoPlayParts;
    ofxMinimalToggle uiBtnChronoAnimParts;
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
    
    // OpenNI ----
    ofxNI2::Device *oniDevice;
    ofxNI2::IrStream oniIr;
    ofxNI2::ColorStream oniColor;
    ofxNI2::DepthStream oniDepth;
    
    
    
};

