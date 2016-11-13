#include "ofApp.h"



#pragma mark -  Setup Functions

//--------------------------------------------------------------
void ofApp::setup(){
    
    appInitStartTime = ofGetElapsedTimeMillis();
    
    // initializing vars ----------------------------------------------
    initAppVars();
    
    // app local settings --------------------------------------------
    maxLoadMeshNum = 2000;      //  2000  100
    skipLoadFrame = 10;         // 1  10
    mapDataColumns = 16;
    colorMode = 1;
    viewerMode = 1;
    
    useOpenNi = false;
    dualColorSystem = true;           // 同じモデルデータを２つのライブラリで同時に読み込み、切り替えながら表示します。２倍動作に時間がかかり、メモリ消費も２倍です。
    loadPictureMode = false;
    loadVertexColorObj = true;         // trueにすると頂点カラー対応（テクスチャ非対応）のライブラリ用にモデルを別に読み込みます　メモリを大量に消費します。
    dataLoadOnAppBoot = true;
    dispPlayControl = true;

    defaultMeshDataDirPath = "/Users/artdkt/Desktop/3dscan_data_for0630/artdkt_structure3d";
    

    // Get File load path --------------------------------------------------------------------------------
    ofFileDialogResult fileDialogResult = ofSystemLoadDialog("Select Time-based 3D Record base directory.", true, defaultMeshDataDirPath);
    
    meshDataDirPath = fileDialogResult.getPath();
    
    // Read Map File ----------------------------------------------------------------
    loadMapFile(meshDataDirPath);
    
    // fonts load --------------------------------------------------------------
    font.loadFont("hira.otf", 38);
    fontSmall.loadFont("hira.otf", 20);
    fontLarge.loadFont("hira.otf", 100);
    fontDebugPrint.loadFont("hira.otf", 12);
    fontMyGui.loadFont("hira.otf", 11);
    ofxGuiSetFont("hira.otf", 11, true, true);
    
    // FBO ------------------------------------------------------------------------
    fboFront.allocate(640, 480, GL_RGBA);
    fboSide.allocate(640, 480, GL_RGBA);
    fboTop.allocate(640, 480, GL_RGBA);
    fboCam.allocate(640, 480, GL_RGBA);
    
    // myGuiSetup ----------------------------------------------------------------------
    myGuiSetup();
    
    // OfxGUI setup -----------------------------------------------------------------
    setupOfxGui();
    
    // OpenNI setup ----------------------------------------------
    setupOpenNi();
    
    // ---------------------------------------------
    // 全力再生設定
    ofSetVerticalSync(false);
    ofSetFrameRate(0);
    
    //uiPlayMode = playMode;
    
    // time set on finish app init ------------------------------------------------
    appInitEndTime = ofGetElapsedTimeMillis();
    playStartDateTime = ofGetElapsedTimeMillis();

    ::time(&unixTimeOnOfStarted);

}
// setup end ----------------------------------------------------------------------------------

void ofApp::initAppVars() {
    
    selectMeshId = 0;
    frameCount = 0;
    playCount = 0;
    playMode = 0;   // 1:timebased 0: frame
    playSeekTime = 0;
    nowPlayTime = 0;
    seekbarAddTime = 0;
    totalMaxMeshNum = 0;
    startPlayMeshAnimNum = 0;
    playStartPrevPos = 0;
    totalLoadedModelNum = 0;
    maxLoadedMeshNumInAllMesh = 0;
    
    loadModelDirName = "";
    
    uiBtnDispWindow = true;
    dispDebugInfoFlag = true;
    dispAllUiFlag = true;
    dispPlayControlUiFlag = true;
    loopPlay = true;
    onPlay = true;
    prevFramePlayState = true;
    mapFileExists = false;

    prevSelectModel = -1;
    
    scanGpsDataMinLat = 1000;
    scanGpsDataMinLong = 1000;
    scanGpsDataMaxLat = 0;
    scanGpsDataMaxLong = 0;
    scanUnixTimeAllItemMin = LONG_MAX;
    scanUnixTimeAllItemMax = LONG_MIN;
    
}

void ofApp::settingAppVarsOnBoot() {
    
}


#pragma mark - GUI Setup
void ofApp::setupOfxGui() {
    
    // Set GUI parts -------------------------------------------------------------------------------
    ofxGuiSetDefaultWidth(300);     // ウィンドウ全体幅決め？
    ofxGuiSetTextPadding(10);
    ofxGuiSetFillColor(ofColor(255,0,0));
    ofxGuiSetBackgroundColor(ofColor(0,255,0));
    
    
    // Play Control ----------------------------------------------------------------
    guiPlayControlBar.setup("PlayControlBar");
    guiPlayControlBar.setPosition(0, ofGetHeight()-100);
    guiPlayControlBar.setSize(ofGetWidth(), 50);
    guiPlayControlBar.setDefaultWidth(70);
    guiPlayControlBar.setAlignHorizontal();        // ボタンを横並びにする
    guiPlayControlBar.setWidthElements(100);
    guiPlayControlBar.setShowHeader(false);
    guiPlayControlBar.add(uiBtnPlayPauseParts.setup(uiBtnPlayPause.set("Play", true), 70, 50) );
    guiPlayControlBar.add(uiBtnLoopPlay.setup("Loop", true, 80, 20));
    
    guiPlayControlMenu.setWidthElements(80);
    guiPlayControlMenu.setDefaultWidth(80);
    guiPlayControlMenu.setup("PlayControlMenu");
    guiPlayControlMenu.setPosition(400, ofGetHeight()-50);
    guiPlayControlMenu.setSize(ofGetWidth(), 50);
    guiPlayControlMenu.setAlignHorizontal();        // ボタンを横並びにする
    guiPlayControlMenu.setShowHeader(false);
    guiPlayControlMenu.add(uiMeshDrawType.setup("mesh", 0, 0, 2));
    guiPlayControlMenu.add(uiColorMode.setup("color", 1, 0, 1));
    guiPlayControlMenu.add(uiPlayMode.setup("play", 2, 0, 2));
    guiPlayControlMenu.add(uiGpsMapMode.setup("map", 0, 0, 1));
    guiPlayControlMenu.add(new ofxGuiSpacer(10));
    guiPlayControlMenu.add(uiBtnGrid.setup("Grid", true, 80, 20));
    guiPlayControlMenu.add(uiBtnDebugInfo.setup("Info", false, 80, 20));
    guiPlayControlMenu.add(uiBtnDispWindowParts.setup(uiBtnDispWindow.set("Window", false), 80, 20) );
    guiPlayControlMenu.add(uiBtnOrtho.setup("Ortho", false, 80, 20));
    guiPlayControlMenu.add(new ofxGuiSpacer(10));
    guiPlayControlMenu.add(uiBtnReset.setup("Reset", 80, 20));
    
    
    // Debug Window gui ----------------------------------------------------------------
    gui.setDefaultWidth(300);
    gui.setup("settings");
    //gui.setFillColor(<#const ofColor &color#>)
    //gui.setDefaultBackgroundColor(ofColor(255,0,0,224));
    gui.setWidthElements(300);
    gui.setPosition(0, 0);
    gui.setDefaultHeight(25);
    //gui.setBackgroundColor(ofColor(0,0,0,32));
    gui.setShowHeader(false);
    
    gui.add(uiThumbnailIconDistance.setup("thumbnailIconDistance", 0, 0, 5000));
    gui.add(uiIconNumX.setup("iconNumX", 4, 1, 8));
    //gui.add(uiColorMode.setup("colorMode", 1, 0, 1));
    gui.add(uiFramerate.setup("framerate", 60, 5, 60));
    gui.add(uiBtnLight.setup("Light on/off", true, 40, 25));
    gui.add(uiBtnTurnMesh.setup("TurnMesh", true, 40, 25));
    gui.add(uiBtnTimerControl.setup("TimerControl", false, 40, 25));
    gui.add(uiTestSlider.setup("TestSlider", 0 ,  -10000, 10000));
    gui.add(uiBtnSelectReset.setup("quit", 40, 25));
    gui.setWidthElements(300);
    
    
    // Map Edit Gui ---------------------------------------------------------------------
    guiMapEdit.setDefaultWidth(300);
    //guiMapEdit.setDefaultBackgroundColor(ofColor(255,0,0,224));
    guiMapEdit.setup("MapEdit");
    guiMapEdit.setDefaultHeight(25);
    guiMapEdit.setPosition(0, 0);
    guiMapEdit.setWidthElements(300);
    guiMapEdit.setShowHeader(false);
    //guiMapEdit.setDefaultHeight(30);
    guiMapEdit.add(uiEditPosX.setup("posX",0,-5000,5000));
    guiMapEdit.add(uiEditPosY.setup("posY",0,-5000,5000));
    guiMapEdit.add(uiEditPosZ.setup("posZ",0,-5000,5000));
    guiMapEdit.add(uiEditRotX.setup("rotX",0,-360,360));
    guiMapEdit.add(uiEditRotY.setup("rotY",0,-360,360));
    guiMapEdit.add(uiEditRotZ.setup("rotZ",0,-360,360));
    guiMapEdit.add(uiEditScale.setup("scale",100,0,5000));
    guiMapEdit.add(uiEditReadStartFrame.setup("FlipV",0,0,2));
    guiMapEdit.add(uiEditReadEndFrame.setup("FlipH",0,0,2));
    guiMapEdit.add(uiEditModelSelector.setup("modelSel",0,0,100));
    guiMapEdit.add(uiEditDisplayFlag.setup("display",0,0,2));
    
    
    // -------------------------------------------------------------------
    prevPosX = position->x;
    prevPosY = position->y;
    
    uiBtnReset.addListener(this, &ofApp::resetCam);

    // Page Settngs ------------------------------------------------------
    guiPage.setup("page1");
    guiPage.setSize(300, 300);
    guiPage.add(&gui);
    guiPage2.setup("page2");
    guiPage2.add(&guiMapEdit);
    
    guiTabbedPages.setup("tabbed pages", "", 100);
    guiTabbedPages.setSize(300, 550);
    guiTabbedPages.setTabWidth(70);
    guiTabbedPages.setTabHeight(30);
    guiTabbedPages.add(&guiPage);
    guiTabbedPages.add(&guiPage2);
    
    // GUI settings end --------------------------------

}

void ofApp::setupOpenNi() {
    
    if (useOpenNi) {
        oniDevice = new ofxNI2::Device;
        oniDevice->setup();
        if (oniDepth.setup(*oniDevice))
        {
            oniDepth.setSize(320, 240);
            oniDepth.setFps(30);
            oniDepth.start();
        }
        
        if (oniIr.setup(*oniDevice)) // only for xtion device (OpenNI2-FreenectDriver issue)
        {
            oniIr.setSize(320, 240);
            oniIr.setFps(30);
            oniIr.start();
        }
        
        if (oniColor.setup(*oniDevice)) // only for kinect device
        {
        		oniColor.setSize(320, 240);
        		oniColor.setFps(60);
        		oniColor.start();
        }
        
    }

}

void ofApp::myGuiSetup() {

    guiPlayControlBar.setPosition(0, ofGetHeight()-100);
    guiPlayControlMenu.setPosition(guiPlayControlMenu.getPosition().x, ofGetHeight()-50);
    
    myGuiMain = ofRectangle(0, ofGetHeight()-100, ofGetWidth(), 100);
    mainView = ofRectangle(0, 0, ofGetWidth(),ofGetHeight() - myGuiMain.height);
    
    myGuiMainMenuDiff = ofRectangle(0, 50, ofGetWidth(), 50);
    myGuiMainTimebarDiff = ofRectangle(0, 0, ofGetWidth(), 50);
    
    myGuiSeekBar = ofRectangle(200, ofGetHeight()-100, ofGetWidth()-400, 50);
    
    myGuiMainMenu = myGuiMain;
    myGuiMainMenu.y += 50;
    myGuiMainMenu.height = 50;
    
    myGuiMainTimebar = myGuiMain;
    myGuiMainTimebar.setHeight(50);
    
    {
        int ty = (ofGetHeight()-myGuiMain.getHeight()) / 2;
        myGuiDetailLeftButton = getSubRect( mainView, ofRectangle(20, ty-40, 80,  80) );
        myGuiDetailRightButton = getSubRect( mainView, ofRectangle(ofGetWidth()-100, ty-40, 80, 80 ) );
    }
    myGuiDispGuiToggle = getSubRect( myGuiMainMenu, ofRectangle(ofGetWidth()-50, ofGetHeight()-50, 50, 50) );
    
    myGuiPlayButton = getSubRect( myGuiMainMenu, ofRectangle(0,0,50,50) );

}





#pragma mark -  Update Functions

// ///////////////////////////////////////////////////////////////////////////////////////////////////
//   Update Functions
// ///////////////////////////////////////////////////////////////////////////////////////////////////

void ofApp::update(){
    
    // Map Edit関係更新
    {
        int i = uiEditModelSelector;
        
        // load / Vars -> GUI
        if (i != prevSelectModel) {
            uiEditPosX = mapNum[i][0];
            uiEditPosY = mapNum[i][1];
            uiEditPosZ = mapNum[i][2];
            uiEditRotX = mapNum[i][3];
            uiEditRotY = mapNum[i][4];
            uiEditRotZ = mapNum[i][5];
            uiEditScale = mapNum[i][6];
            uiEditReadStartFrame = mapNum[i][7];
            uiEditReadEndFrame = mapNum[i][8];
            uiEditDisplayFlag = mapNum[i][9];
        }
        
        // save / GUI -> Vars
        mapNum[i][0] = uiEditPosX;
        mapNum[i][1] = uiEditPosY;
        mapNum[i][2] = uiEditPosZ;
        mapNum[i][3] = uiEditRotX;
        mapNum[i][4] = uiEditRotY;
        mapNum[i][5] = uiEditRotZ;
        mapNum[i][6] = uiEditScale;
        mapNum[i][7] = uiEditReadStartFrame;
        mapNum[i][8] = uiEditReadEndFrame;
        mapNum[i][9] = uiEditDisplayFlag;
        
        prevSelectModel = i;
    }
    
    // マウス座標で操作モード切替
    {
        if (mouseY >= myGuiMain.getTop()) {
            uiBtnTimerControl = true;
        } else {
            uiBtnTimerControl = false;
        }
        
        if (uiBtnTimerControl) {
            eCam.disableMouseInput();
        } else {
            eCam.enableMouseInput();
        }
    }
    
    // 再生停止関係
    if (prevFramePlayState == false && uiBtnPlayPause) {
        playStartPrevPos = nowPlayTime;
        playStartDateTime = ofGetElapsedTimeMillis();
    }
    
    // シークバー更新処理
    if (frameCount > 1) {
        
        if (!uiBtnLoopPlay) {       // Stop Pattern
            
            if (uiPlayMode == 1) {      // time-based
                if (nowPlayTime >= totalScanTimeRecordMaxTime) {
                    uiBtnPlayPause = false;
                    nowPlayTime = totalScanTimeRecordMaxTime-1;
                    //playCount = totalMaxMeshNum;
                }
                
            } else if (uiPlayMode == 2) {      // datetime-based sync
                
                if (nowPlayTime >= totalScanTimeRecordMaxTime) {
                    uiBtnPlayPause = false;
                    nowPlayTime = totalScanTimeRecordMaxTime-1;
                    //playCount = totalMaxMeshNum;
                }
                
            } else {                   // frame-based
                
                if (viewerMode == 0) {
                    if (playCount >= maxMeshNumList[selectMeshId] ) {
                        uiBtnPlayPause = false;
                        playCount = totalMaxMeshNum-1;
                    }
                    
                } else if (viewerMode == 1) {
                    if (playCount >= totalMaxMeshNum) {
                        uiBtnPlayPause = false;
                        playCount = totalMaxMeshNum-1;
                    }
                }
            }
            
        } else {                    // Loop Pattern
            
            if (uiPlayMode == 1) {      // time-based
                nowPlayTime %= totalScanTimeRecordMaxTime;
            } else if (uiPlayMode == 2) {
                if (viewerMode == 1) {
                    nowPlayTime %= totalScanTimeRecordMaxTime;
                } else if (viewerMode == 2) {
                    
                } else if (viewerMode == 0) {
                    
                }
            } else {            // frame-based
                if (viewerMode == 1) {
                    playCount %= (totalMaxMeshNum / skipLoadFrame);
                } else if (viewerMode == 0) {
                    playCount %= (maxMeshNumList[selectMeshId] / skipLoadFrame);
                }
            }
            
        }
        
    }

}


void ofApp::updateSeekBar(int x, int y) {
    
    if (uiBtnTimerControl) {
        
        // シークバー
        if (myGuiSeekBar.inside(x, y)) {
            
            double barWidth = myGuiSeekBar.getWidth();
            double barX =  myGuiSeekBar.getLeft();//100;//ofGetWidth() / 10;
            double progressPosX = mouseX - barX;
            
            // 同期再生
            if (uiPlayMode == 2) {
                
                long seekbarCalcTime = (int)((progressPosX / barWidth) * (scanUnixTimeAllItemMax - scanUnixTimeAllItemMin));
                
                seekbarAddTime =
                    (scanUnixTimeAllItemMax - scanUnixTimeAllItemMin)
                     - ((ofGetElapsedTimeMillis() - scanUnixTimeAllItemMin)
                        - (seekbarCalcTime - scanUnixTimeAllItemMin));
                
                // 総再生時間(ms)から「現在時刻からデータの再生開始時刻を引いたもの(ms)」を引き、さらに「シークバーの示す再生時間からデータの再生開始時刻を引いたもの」を引く（追加時間なので引く）
                
            // 実時間再生
            } else if (uiPlayMode == 1) {
                
                seekbarAddTime =
                    totalScanTimeRecordMaxTime
                    - ((ofGetElapsedTimeMillis() - scanUnixTimeAllItemMin)
                    - (int)((progressPosX / barWidth) * totalScanTimeRecordMaxTime));
                
            // フレーム再生
            } else {
                
                playCount = (int)((progressPosX/skipLoadFrame / barWidth ) * totalMaxMeshNum );
                
            }
        }
        
    }

}


#pragma mark -  Draw Functions

// ///////////////////////////////////////////////////////////////////////////////////////////////////
//   Draw Functions
// ///////////////////////////////////////////////////////////////////////////////////////////////////

void ofApp::draw(){

    ofSetFrameRate(uiFramerate);
    ofEnableSmoothing();

    if (frameCount == 1) {  // app start
        dataLoad();
    }
    
    // init vars --------------------------------------
    displayTotalVertices = 0;
    
    // BG ---------------------------------------------
    ofBackground(240, 240, 240);
    ofSetColor(255,255,255,255);

    // display now Loading -----------------------------------------
    if (frameCount == 0) {
        font.drawString("Now Loading...", 500, 400);
    }
    
    // Light Settings ---------------------------------------
    if (uiBtnLight) {
        ofEnableLighting();
        light.enable();
        light.setSpotlight();
        if (uiBtnTurnMesh) {
            //light.setPosition(10000,1000,20000);
            light.setPosition(10000,1000,100000);
            //light.setPosition(000,10000,-10000);
            //light.setDirectional();
        } else {
            light.setPosition(-1000,1000,200000);
            //light.setDirectional();
        }
        //light.setAmbientColor(ofFloatColor(0.2, 0.2, 0.2, 1.0));
        light.setAmbientColor(ofFloatColor(0.3, 0.3, 0.3, 1.0));
        light.setDiffuseColor(ofFloatColor(0.4, 0.4, 0.4));
        light.setSpecularColor(ofFloatColor(1.0, 1.0, 1.0));
        ofEnableDepthTest();
    }
    
    // Camera Settings ----------------------------------------
    eCam.setFarClip( 100000.0f );
    if(frameCount==1){
        resetCamListView();
    }
    
    eCam.begin();
    if (uiBtnOrtho) {
        eCam.enableOrtho();
    } else {
        eCam.disableOrtho();
    }
    
    // base Axis Setting -------------------------------------------
    ofScale(1,-1);      // y-axis reverse! (for fix drawString text flips promblem)

    
    // -----------------------------------------------------------
    
#pragma mark - Draw Mesh
    glPushMatrix();
    
    ofSetColor(255,255,255,255);
    int indexX = mouseX / uiThumbnailIconDistance;
    int indexY = mouseY / uiThumbnailIconDistance;
    
    int playFrameSelector;
    
    for(int i=0; i<modelDataNum; i++) {
        
        if (viewerMode == 0 && selectMeshId != i) {
            //continue;
        }
        
        int counter = playCount;// % maxMeshNumList[i];
        playFrameSelector = 0;
        
        
        if (frameCount >= 1) {
            modelFlagList[i] = 0;
            
            // 再生位置シーク処理
            // 同期再生
            if (uiPlayMode == 2) {
                
                if (maxMeshNumList[i] >= 2 && scanTimeRecordMaxTime[i] > 0) {
                    
                    // 再生時
                    if (uiBtnPlayPause) {
                        nowPlayTime =  ( (ofGetElapsedTimeMillis() + seekbarAddTime) % (scanUnixTimeAllItemMax - scanUnixTimeAllItemMin)) + playStartPrevPos;     // 0 start realtime incremental num (msec)
                    }
                    cout << "playStartDateTime: " << playStartDateTime << " playStartPrevPos:" << playStartPrevPos << endl;
                    
                    bool dispFlag = false;
                    long virtualPlayUnixTime = nowPlayTime + scanUnixTimeAllItemMin;
                    
                     // scan play frame by time
                     for (int j=0; j<maxMeshNumList[i]; j++) {
                         if (virtualPlayUnixTime >= scanUnixTimeLongIntList[i][j] &&
                             virtualPlayUnixTime <= scanUnixTimeLongIntList[i][j+1]  ) {
                             playFrameSelector = j;
                             dispFlag = true;
                             break;
                         }
                     }
                    
                    if (!dispFlag){
                        playFrameSelector = 0;
                        modelFlagList[i] = 2;           //その時間にデータがなかった場合は、非表示ともまた違う処理ができるようにしておく
                    }
    
                }
                
            // 実時間再生
            } else if (uiPlayMode == 1) {
                
                // get play frame from play time ------------------------------------------------------------------
                if (maxMeshNumList[i] >= 2 && scanTimeRecordMaxTime[i] > 0) {       // error kaihi
                    
                    // 再生時
                    if (uiBtnPlayPause) {
                        nowPlayTime =  ofGetElapsedTimeMillis() - playStartDateTime + playStartPrevPos;
                    }
                    
                    int passedTime = nowPlayTime % scanTimeRecordMaxTime[i];
                    
                    for (int j=0; j<maxMeshNumList[i]; j++) {
                        if (passedTime <= scanTimeRecordList[i][j+1][1]) {
                            playFrameSelector = j;
                            break;
                        }
                    }
                    
                } else {

                    playFrameSelector = 0;
                    
                }
            
            // フレーム再生
            } else {        // uiPlayMode == 0  frame base play
                
                playFrameSelector = counter;
                
            }
            
        }
        
        
        // 描画 --------------------------------------------------------
        if (viewerMode == 0) {
            
            drawDetailView(i, playFrameSelector);
            
        } else if (viewerMode <= 1) {
            
            drawListView(i, playFrameSelector);
            
        } else  {
            
            drawMapView(i, playFrameSelector);

        }
        
    }       // for loop
    
    // OpenNi -------------------------------------------------
    drawOpenNi();
    
    glPopMatrix();      // end Mesh Draw
    
    eCam.end();         // end Mesh Draw Cam
 
    
#pragma mark - Other Viewpoint FBO
    
    if (viewerMode == 0) {
        
        glPushMatrix();
        //fboCam.draw(0,160);
        ofScale(0.5,0.5,0.5);
        modelImageList[selectMeshId][playFrameSelector].draw(0,0);
        glPopMatrix();
        
    }
    
    drawUi();
    
    frameCount++;
    
    if (uiBtnPlayPause) {
        playCount++;
    }

}
// Main Draw Meshod end ----------------------------------------------------------------------------------


# pragma mark - Draw Detail view
void ofApp::drawDetailView(int i, int playFrameSelector) {
    
    if (mapNum[i][9]) {
        return;
    }

    displayTotalVertices += meshVertexNumList[i][playFrameSelector];

    /*
    fboCam.begin();
    {
        ofPushMatrix();
        
        //ofPushView();
        
        ofCam.begin();
        //ofCam.reset();
        ofClear(255,255,255, 0);
        ofSetColor(255,0,0,255);
        font.drawString("test", 100,50);
        //ofScale(0.25, 0.25, 0.25);
        asModelObj[i][playFrameSelector].draw(OF_MESH_FILL);
        
        ofCam.end();
        
        //ofPopView();
        ofPopMatrix();
    }
    fboCam.end();
     //asModelObj[i][playFrameSelector].draw(OF_MESH_FILL);
    */
    
    // グリッド表示
    if (uiBtnGrid && i == selectMeshId) {
        float modelSize = modelSceneMax[i].x - modelSceneMin[i].x;
        if (modelSize < 1) {
            drawScaleGrid(modelSize*1000, 100);
        } else {
            drawScaleGrid(modelSize*1000, 1000);
        }
    }
    
    glPushMatrix();
    
    // Draw Model Name
    {
        glPushMatrix();  //
        ofSetColor(255,255,255,255);
        //glTranslatef(180, 400, 700); //
        glTranslatef(-0,0,2);
        
        ofDisableLighting();        //
        fontLarge.drawString(dataDirNameList[i],0,0);        // display model name
        ofEnableLighting();
        
        //font.drawString(to_string(asModelObj[i][counter].getNumMeshes()),600,500);
        glPopMatrix();
    }
    
    glTranslatef(mapNum[i][0], mapNum[i][1], mapNum[i][2]);
    
    // fix model direction
    glRotatef(-90, 1, 0, 0);    // turn model
    ofScale(1, 1, -1);      // fix model direction
    ofScale(-1, -1, 1);      // fix model direction
    ofScale(1000, 1000, 1000);
    asModelObj[i][playFrameSelector].setScaleNormalization(false);
    
    ofRotateX(mapNum[i][3]);
    ofRotateY(mapNum[i][4]);
    ofRotateZ(mapNum[i][5]);
    
    double centerX = modelSceneMin[i].x + (modelSceneMax[i].x - modelSceneMin[i].x) / 2;
    double centerY = modelSceneMin[i].y + (modelSceneMax[i].x - modelSceneMin[i].x) / 2;
    double centerZ = modelSceneMin[i].z + (modelSceneMax[i].x - modelSceneMin[i].x) / 2;
    ofTranslate(centerX, centerY, -centerZ);
    
    if (mapNum[i][7]) {
        glRotatef(180, 0, 1, 0);
        ofTranslate(0,0,0);
    }
    if (mapNum[i][8]) {
        glRotatef(180, 1, 0, 0);
    }
    if (mapNum[i][6]) {
        glRotatef(180, 0, 0, 1);
    }
    

    if (uiColorMode) {
        asModelObj[i][playFrameSelector].enableTextures();
    } else {
        asModelObj[i][playFrameSelector].disableTextures();
    }

    if (uiMeshDrawType == 1) {
        ofSetLineWidth(1);
        asModelObj[i][playFrameSelector].draw(OF_MESH_WIREFRAME);
    } else if (uiMeshDrawType == 2) {
        glPointSize(4);
        //ofBlendMode(OF_BLENDMODE_ALPHA);
        asModelObj[i][playFrameSelector].draw(OF_MESH_POINTS);
    } else {
        asModelObj[i][playFrameSelector].draw(OF_MESH_FILL);
    }
    
    glPopMatrix();
    
    
    // Draw Model Picture --------------------------
    glPushMatrix();
    {
        glRotatef(90, 1, 0, 0);
        ofTranslate(0,0,1000);
        ofScale(10,10,10);
        modelImageList[i][playFrameSelector].draw(0,0);
    }
    glPopMatrix();

}


# pragma mark - Draw List view
void ofApp::drawListView(int i, int playFrameSelector) {
    
    if (uiBtnGrid && i == 0) {
        drawScaleGrid(10000, 1000);
    }
    
    if (uiGpsMapMode == 0) {
        
        drawListViewNormal(i, playFrameSelector);
        
    } else if (uiGpsMapMode == 2) {
        
        //drawListViewGpsMap(i, playFrameSelector);
        
    } else if (uiGpsMapMode == 3) {
    
        //drawListViewGpsMapWalkThru(i, playFrameSelector);
        
    } else if (uiGpsMapMode == 1) {
        
        drawListViewTrackingMap(i, playFrameSelector);
        
    }
    

}

void ofApp::drawListViewNormal(int i, int playFrameSelector) {

    if (mapNum[i][9]) {
        return;
    }
    
    displayTotalVertices += meshVertexNumList[i][playFrameSelector];
    
    glPushMatrix();
    
    // リスト配置分
    glTranslatef(((i%uiIconNumX))*uiThumbnailIconDistance, (i/uiIconNumX)*uiThumbnailIconDistance, 0);
    
    //glRotatef(180, 0, 1, 0);        //
    
    int gridSize = 1000;
    int gridDiv = 10;
    int gridRowSize = gridSize / gridDiv;
    
    // Draw Model Name
    {
        glPushMatrix();  //
        
        ofSetColor(64,64,64,255);
        glTranslatef(0,0,1 );      // グリッドの床面に埋もれないように少しだけ持ち上げる
        
        ofDisableLighting();
        ofSetColor(0,0,0);
        fontLarge.drawString(dataDirNameList[i],0,0);        // display model name
        ofEnableLighting();
        
        glPopMatrix();
    }
    
    
    if (mapNum[i][7]) {
        glRotatef(180, 0, 1, 0);
    }
    if (mapNum[i][8]) {
        glRotatef(180, 1, 0, 0);
    }
    if (mapNum[i][6]) {
        glRotatef(180, 0, 0, 1);
        //ofTranslate(0,0,-530);
    }
    
    ofRotateX(mapNum[i][3]);
    ofRotateY(mapNum[i][4]);
    ofRotateZ(mapNum[i][5]);
    
    // fix model direction
    glRotatef(-90, 1, 0, 0);    // turn model
    ofScale(1, 1, -1);      // fix model direction
    ofScale(-1, -1, 1);      // fix model direction
    ofScale(1000, 1000, 1000);
    asModelObj[i][playFrameSelector].setScaleNormalization(false);
    //ofTranslate(0,-0,modelPosZList[i]*1000);        // hosei
    //                ofTranslate(1500,1100,-2500);      // goto center
    
    
    // ---------------------------------------------------
    if (modelFlagList[i] == 0) {            // not effect vertex color object
        ofSetColor(255, 255, 255, 255);
    } else if (modelFlagList[i] == 1) {
        ofSetColor(255, 255, 255, 32);
    } else if (modelFlagList[i] == 2) {
        ofSetColor(0, 255, 0, 64);
    }

    glTranslatef(mapNum[i][0], mapNum[i][1], mapNum[i][2]);
    
    double centerX = modelSceneMin[i].x + (modelSceneMax[i].x - modelSceneMin[i].x) / 2;
    double centerY = modelSceneMin[i].y + (modelSceneMax[i].x - modelSceneMin[i].x) / 2;
    double centerZ = modelSceneMin[i].z + (modelSceneMax[i].x - modelSceneMin[i].x) / 2;
    ofTranslate(centerX, centerY, -centerZ);
    
    if (uiColorMode) {
        asModelObj[i][playFrameSelector].enableTextures();
    } else {
        asModelObj[i][playFrameSelector].disableTextures();
    }
    
    if (uiMeshDrawType == 1) {
        ofSetLineWidth(1);
        asModelObj[i][playFrameSelector].draw(OF_MESH_WIREFRAME);
    } else if (uiMeshDrawType == 2) {
        asModelObj[i][playFrameSelector].draw(OF_MESH_POINTS);
    } else {
        asModelObj[i][playFrameSelector].draw(OF_MESH_FILL);
    }
    
    glPopMatrix();

}

/*
void ofApp::drawListViewGpsMap(int i, int playFrameSelector) {
    
    //cout << "maxMeshNumList" << maxMeshNumList[i] << endl;
    
    for(int z=0; z<maxMeshNumList[i]; z++) {
        displayTotalVertices += meshVertexNumList[i][z];
        
        glPushMatrix();
        
        playFrameSelector = z;
        
        double latScale = 10000*uiTestSlider/100;
        double longScale = 10000*uiTestSlider/100;
        
        double posX = (scanGpsDataList[i][z][1] - scanGpsDataMinLong) * longScale;
        double posY = (scanGpsDataList[i][z][0] - scanGpsDataMinLat) * latScale;
        
        //cout << "posX: " << posX << " posY: " << posY << endl;
        
        
        if (uiBtnTurnMesh) {
            glRotatef(-90, 1, 0, 0);
            
        }
        //ofTranslate(0,-1*modelHeightList[i]*1000,0);
        //ofTranslate(0,-0,modelPosZList[i]*1000);        // hosei
        
        ofScale(100, 100, 100);  // temp debug
        
        // 6/29
        ofScale(1, 1, -1);      // fix model direction
        
        ofTranslate(0,0,posY);
        ofTranslate(posX,0,0);
        
        if (modelFlagList[i] == 0) {            // not effect vertex color object
            ofSetColor(255, 255, 255, 255);
        } else if (modelFlagList[i] == 1) {
            ofSetColor(255, 255, 255, 32);
        } else if (modelFlagList[i] == 2) {
            ofSetColor(0, 255, 0, 64);
        }
        ofSetColor(255, 255, 255, 255);
        
        if (mapNum[i][9] == 0) {
            
            if (uiMeshDrawType == 1) {
                ofSetLineWidth(1);
                asModelObj[i][playFrameSelector].draw(OF_MESH_WIREFRAME);
            } else if (uiMeshDrawType == 2) {
                glPointSize(5);
                
                asModelObj[i][playFrameSelector].draw(OF_MESH_POINTS);
            } else {
                //asModelObj[i][counter].drawFaces();
                asModelObj[i][playFrameSelector].draw(OF_MESH_FILL);
            }
            
        }
        glPopMatrix();
        
    }

}

void ofApp::drawListViewGpsMapWalkThru(int i, int playFrameSelector) {
    
    glPushMatrix();
    
    cout << "maxMeshNumList" << maxMeshNumList[i] << endl;
    for(int z=0; z<maxMeshNumList[i]; z++) {
        
        glPushMatrix();
        
        playFrameSelector = z;
        
        // 緯度（上下？）経度に適当な係数を掛けると、ある程度正確な座標になる　ここまでは一応座標取れて、配置できている。
        double latScale = 100*uiTestSlider;
        double longScale = 100*uiTestSlider;
        
        // 北が緯度高く90、東ガ経度高く９０
        double posX = (scanGpsDataList[i][z][1] - scanGpsDataMinLong) * longScale*1000;
        double posY = (scanGpsDataList[i][z][0] - scanGpsDataMinLat) * latScale*1000;
        
        cout << "posX: " << posX << " posY: " << posY << endl;
        
        // 座標移動
        ofTranslate(0,-posY,0);
        ofTranslate(0,0,0);
        
        if (uiBtnTurnMesh) {
            glRotatef(-90, 1, 0, 0);
            
        }
        ofTranslate(0,-1*modelHeightList[i]*1000,0);
        ofTranslate(0,-0,modelPosZList[i]*1000);        // hosei
        
        ofScale(1000, 1000, 1000);  // temp debug
        
        // 6/29
        ofScale(1, 1, -1);      // fix model direction
        
        long nowPlayTimeTemp = nowPlayTime + scanUnixTimeAllItemMin;
        long nowPlayTimeForSeek = nowPlayTimeTemp / 1000;               // 秒単位に
        
        //cout << "nowPlayTimeTemp" << nowPlayTime << "nowPlayTimeTemp: " << nowPlayTimeTemp << endl;
        
        double camPosY = 0;
        double currentCamPos = INT_MAX;
        double currentSpeed = 0;
        for(int p=0; p<maxMeshNumList[i]-1; p++) {
            
            //cout << "nowPlayTimeTemp: " << nowPlayTimeTemp << " scanUnixTimeLongIntList[i][p+1]: " <<  scanUnixTimeLongIntList[i][p+1] << endl;
            
            double timeA = scanUnixTimeLongIntList[i][p];
            double timeB = scanUnixTimeLongIntList[i][p+1];
            
            if ((nowPlayTimeTemp >= timeA) && (nowPlayTimeTemp <= timeB)) {
                
                double b2pTime = timeB - timeA;     // between two points time
                double elapsedTime = nowPlayTimeTemp - timeA;
                double normalizationElapseTime = elapsedTime / b2pTime;
                
                double b2pLatDistance = (scanGpsDataList[i][p+1][0] - scanGpsDataList[i][p][0]);
                
                double elapedLatDistance = b2pLatDistance * normalizationElapseTime;
                camPosY = (elapedLatDistance + (scanGpsDataList[i][p][0] - scanGpsDataMinLat)) * latScale * 1000;
                
                //cout << "camPosY(setted!): " << camPosY << endl;
                break;
                
            }
        }
        
        //camPosY = 1*ofGetFrameNum();
        
        eCam.reset();
        
        eCam.setPosition(5000,camPosY,1200);
        eCam.lookAt(ofVec3f(5000, -1000+10000+camPosY, 1200), ofVec3f(0,0,1));
        
        if (modelFlagList[i] == 0) {            // not effect vertex color object
            ofSetColor(255, 255, 255, 255);
        } else if (modelFlagList[i] == 1) {
            ofSetColor(255, 255, 255, 32);
        } else if (modelFlagList[i] == 2) {
            ofSetColor(0, 255, 0, 64);
        }
        ofSetColor(255, 255, 255, 255);
        
        if (mapNum[i][9] == 0) {
            
        
        
        }
        glPopMatrix();
        
    }
    glPopMatrix();

}
*/

void ofApp::drawListViewTrackingMap(int i, int playFrameSelector) {
    
    for(int z=0; z<maxMeshNumList[i]; z++) {
        
        if (mapNum[i][9]) {
            continue;
        }
        
        glPushMatrix();
        
        displayTotalVertices += meshVertexNumList[i][z];
        playFrameSelector = z;
        
        // fix model direction
        glRotatef(-90, 1, 0, 0);    // turn model
        ofScale(1, 1, -1);      // fix model direction
        ofScale(-1, -1, 1);      // fix model direction
        ofScale(1000, 1000, 1000);
        asModelObj[i][playFrameSelector].setScaleNormalization(false);
        
        ofVec3f tr = modelMatrixList[z].getTranslation();
        double posX = tr.x * (1000 + uiTestSlider);
        double posY = tr.z * (1000 + uiTestSlider);
        double posZ = tr.y * (1000 + uiTestSlider);
        ofTranslate(posX,posZ,posY);
        cout << "posX: " << posX << " posY: " << posY << endl;
    
    
        if (uiColorMode) {
            asModelObj[i][playFrameSelector].enableTextures();
        } else {
            asModelObj[i][playFrameSelector].disableTextures();
        }

        if (uiMeshDrawType == 1) {
            ofSetLineWidth(1);
            asModelObj[i][playFrameSelector].draw(OF_MESH_WIREFRAME);
        } else if (uiMeshDrawType == 2) {
            glPointSize(5);
            
            asModelObj[i][playFrameSelector].draw(OF_MESH_POINTS);
        } else {
            //asModelObj[i][counter].drawFaces();
            asModelObj[i][playFrameSelector].draw(OF_MESH_FILL);
        }
            
    
        glPopMatrix();
        
    }
    
    // Draw Moving Line
    {
        glPushMatrix();
        
        ofSetLineWidth(5);
        ofSetColor(0,64,255);
        for(int z=0; z<maxMeshNumList[i]-1; z++) {
            ofMatrix4x4 matrixA = modelMatrixList[z];
            ofMatrix4x4 matrixB = modelMatrixList[z+1];
            
            ofVec3f posA = matrixA.getTranslation();
            ofVec3f posB = matrixB.getTranslation();
            
            ofDrawLine(posA.x, posA.y, posA.z, posB.x, posB.y, posB.z);
            
            //cout << "posX: " << posX << " posY: " << posY << endl;
        }
        
        glPopMatrix();
    }
    
}


# pragma mark - Draw Other view
void ofApp::drawMapView(int i, int playFrameSelector) {
    
    glPushMatrix();
    
    glRotatef(180, 0, 1, 0);        // ‚Ä∞‚àè√§‚Ä∞‚àè√£√ä√±œÄ√Ç√™√´√Å√µ¬•‚Äû√Ö√¥
    
    glTranslatef(mapNum[i][0], mapNum[i][1], mapNum[i][2]);
    ofRotateX(mapNum[i][3]);
    ofRotateY(mapNum[i][4]);
    ofRotateZ(mapNum[i][5]);
    
    glTranslatef(333*mapNum[i][6]/100.0, -333*mapNum[i][6]/100.0, 0);       //
    
    ofScale(mapNum[i][6]/100.0, mapNum[i][6]/100.0, mapNum[i][6]/100.0);
    
    if (uiBtnTurnMesh) {
        glRotatef(-90, 1, 0, 0);
        
    }
    
    if (mapNum[i][9] == 0) {
        
        if (uiMeshDrawType == 1) {
            asModelObj[i][playFrameSelector].draw(OF_MESH_WIREFRAME);
        } else if (uiMeshDrawType == 2) {
            asModelObj[i][playFrameSelector].draw(OF_MESH_POINTS);
        } else {
            asModelObj[i][playFrameSelector].drawFaces();
            //asModelObj[i][counter].draw(OF_MESH_FILL);
            
        }
    }
    
    glPopMatrix();

}

#pragma mark - Draw UI
void ofApp::drawUi() {

    ofDisableLighting();
    light.disable();
    ofDisableDepthTest();
    
    if (dispAllUiFlag) {
        
        if (dispPlayControl) {
            drawPlayControlMenu();
        }
        
        if (uiBtnDebugInfo) {
            drawDebugInfo();
        }
        
        drawSeekBar();
        
        drawViewerModeChanger();
        
        if (viewerMode == 0) {
            drawModelLRSelector();
        }

        
        // draw ofxGUI
        if (uiBtnDispWindow) {
            //guiMapEdit.draw();
            //gui.draw();
            guiTabbedPages.draw();
        }
        if (dispPlayControl) {
            guiPlayControlBar.draw();
            guiPlayControlMenu.draw();
        }
        
    }
    
}

void ofApp::drawSeekBar() {
    
    int barWidth = myGuiSeekBar.getWidth();
    int barX =  myGuiSeekBar.getLeft();//100;//ofGetWidth() / 10;
    int progressPosX;
    
    if (frameCount>1) {
        if (uiPlayMode == 2) {
            progressPosX = (nowPlayTime  * barWidth ) / (scanUnixTimeAllItemMax - scanUnixTimeAllItemMin);
        } else if (uiPlayMode == 1) {
            progressPosX = (nowPlayTime  * barWidth ) / totalScanTimeRecordMaxTime;
        } else {
            
            if (viewerMode == 0) {
                progressPosX = (playCount * skipLoadFrame * barWidth ) / maxMeshNumList[selectMeshId];
            } else if (viewerMode == 1) {
                progressPosX = (playCount * skipLoadFrame * barWidth ) / totalMaxMeshNum;
            }
        }
    }
    
    ofSetColor(64,64,64, 64);
    ofSetLineWidth(3);
    ofDrawLine(barX, myGuiSeekBar.getTop() + myGuiSeekBar.getHeight()/2, barX+barWidth,  myGuiSeekBar.getTop() + myGuiSeekBar.getHeight()/2);
    ofDrawCircle(progressPosX+barX+2,  myGuiSeekBar.getTop() + myGuiSeekBar.getHeight()/2 + 2, 16);
    ofSetColor(255,255,255);
    ofDrawCircle(progressPosX+barX,  myGuiSeekBar.getTop() + myGuiSeekBar.getHeight()/2, 14);

}

void ofApp::drawPlayControlMenu() {
    stringstream tSs;
    
    int pX = 40;
    int pY = 80;
    int fSize = 10;
    int lineHeight = fSize*2;
    
    ofSetColor(0, 0, 0, 32);
    ofDrawRectangle(myGuiMain);
    
    pY = myGuiMain.getTop()-50;
    
    // upper menu display -----------------------------------------------------------------------------
    
    ofSetColor(64,64,64,255);
    tSs.str("");
    tSs << fixed << setprecision(1) << ofGetFrameRate() << " fps ";
    fontDebugPrint.drawString(tSs.str(), 20, pY); pY += lineHeight;
    
    tSs.str("");
    tSs <<  "ModelName: " << meshNameList[selectMeshId];
    fontDebugPrint.drawString(tSs.str(), 20, pY); pY += lineHeight;
    
    tSs.str("");
    tSs <<  "DirectoryName: " << loadModelDirName;
    fontDebugPrint.drawString(tSs.str(), 20, pY); pY += lineHeight;
    
    
    // play time display --------------------------------------------------------------------------------
    
    ofSetColor(64,64,64,255);
    if (uiPlayMode == 0) {
        tSs.str("");
        tSs << "" << playCount << " / " << maxMeshNumList[selectMeshId] << " frame";
        fontMyGui.drawString(tSs.str(), myGuiSeekBar.getRight() + 20, myGuiSeekBar.getTop() + 20);
        // tSs << "frame: " << playCount << " /  " << totalMaxMeshNum;
    }
    
    if (uiPlayMode == 1) {
        /*
         if (viewerMode == 0) {
         tSs.str("");
         tSs << "frame: " << playCount << " / " << maxMeshNumList[selectMeshId];
         fontMyGui.drawString(tSs.str(), myGuiSeekBar.getRight() + 20, myGuiSeekBar.getTop() + 20);
         }
         */
        
        tSs.str("");
        tSs << fixed << setprecision(3);
        if (viewerMode == 0) {
            tSs << "" << nowPlayTime/1000.0 << " / " << totalScanTimeRecordMaxTime/1000.0 << " sec";
        } else {
            tSs << "" << nowPlayTime/1000.0 << " / " << scanTimeRecordMaxTime[selectMeshId]/1000.0 << " sec";
        }
        fontMyGui.drawString(tSs.str(), myGuiSeekBar.getRight() + 20, myGuiSeekBar.getTop() + 20);
        
    }
    
    
    if (uiPlayMode == 2) {
        tSs.str("");
        long nowPlayTimeTemp = nowPlayTime + scanUnixTimeAllItemMin;
        long nowPlayTimeForSeek = nowPlayTimeTemp / 1000;
        cout << "nowPlayTimeForSeek: " << nowPlayTimeForSeek << " playStartDateTime:" << playStartDateTime << endl;
        char charDateTime[100];
        //time(&timer);
        struct tm tempTimeStruct;
        memset(&tempTimeStruct,0x00,sizeof(struct tm));               // Initialize important!
        tempTimeStruct = *localtime(&nowPlayTimeForSeek);
        ::strftime(charDateTime, sizeof(charDateTime), "%Y-%m-%d", &tempTimeStruct);
        string strCharDateTime = charDateTime;
        tSs <<  "" << strCharDateTime;
        fontMyGui.drawString(tSs.str(), myGuiSeekBar.getRight() + 20, myGuiSeekBar.getTop() + 20);
        tSs.str("");
        ::strftime(charDateTime, sizeof(charDateTime), "%H:%M:%S", &tempTimeStruct);
        strCharDateTime = charDateTime;
        tSs <<  "" << strCharDateTime << "." << nowPlayTimeTemp%1000;
        fontMyGui.drawString(tSs.str(), myGuiSeekBar.getRight() + 20, myGuiSeekBar.getTop() + 40);
    }

    
}

void ofApp::drawDebugInfo() {
    
    stringstream tSs;
    
    int pX = 40;
    int pY = 80;
    int fSize = 10;
    int lineHeight = 20;
    
    // display Debug Info -------------------------
    ofSetColor(64,64,64,255);
    
    tSs.str("");
    tSs << "FPS: " << fixed << setprecision(1) << ofGetFrameRate() << "fps" << resetiosflags(ios_base::floatfield);
    fontDebugPrint.drawString(tSs.str(), pX, pY); pY += lineHeight;
    
    tSs.str("");
    tSs << "Vertices: " << displayTotalVertices << "pts";
    fontDebugPrint.drawString(tSs.str(), pX, pY); pY += lineHeight;
    
    tSs.str("");
    tSs << "AppInitTime: " << (appInitEndTime - appInitStartTime) << "ms";
    fontDebugPrint.drawString(tSs.str(), pX, pY); pY += lineHeight;
    
    tSs.str("");
    tSs << "modelLoadingTime: " << (modeldataLoadingEndTime - modeldataLoadingStartTime) << "ms";
    fontDebugPrint.drawString(tSs.str(), pX, pY); pY += lineHeight;
    
    tSs.str("");
    tSs << "modelNum: " << modelDataNum;
    fontDebugPrint.drawString(tSs.str(), pX, pY); pY += lineHeight;
    
    tSs.str("");
    tSs << "files: " << modeldataFiles;
    fontDebugPrint.drawString(tSs.str(), pX, pY); pY += lineHeight;
    
    tSs.str("");
    tSs << "totalLoadFileSize: " << (loadFileSizeAll/1000/1000) << "MB";
    fontDebugPrint.drawString(tSs.str(), pX, pY); pY += lineHeight;
    
    tSs.str("");
    tSs << "PlayStartFrame: " << startPlayMeshAnimNum;
    fontDebugPrint.drawString(tSs.str(), pX, pY); pY += lineHeight;
    
    tSs.str("");
    tSs << "maxLoadMeshNum: " << maxLoadMeshNum;
    fontDebugPrint.drawString(tSs.str(), pX, pY); pY += lineHeight;
    
    tSs.str("");
    tSs << "skipLoadFrame: " << skipLoadFrame;
    fontDebugPrint.drawString(tSs.str(), pX, pY); pY += lineHeight;
    
    tSs.str("");
    tSs << "maxLoadedModelNum: " << maxLoadedMeshNumInAllMesh;
    fontDebugPrint.drawString(tSs.str(), pX, pY); pY += lineHeight;
    
    tSs.str("");
    tSs << "mouseX: " << mouseX << " mouseY: " << mouseY << "eCam.x: " << eCam.getX() << " eCam.y: " << eCam.getY() << " eCam.z" << eCam.getZ();
    fontDebugPrint.drawString(tSs.str(), pX, pY); pY += lineHeight;
    
    tSs.str("");
    ofVec3f worldPos = eCam.screenToWorld(ofVec3f(mouseX, mouseY, 0));
    tSs << "worldX: " << worldPos.x << " worldY: " << worldPos.y << " worldZ: " << worldPos.z;
    fontDebugPrint.drawString(tSs.str(), pX, pY); pY += lineHeight;
    
    tSs.str("");
    tSs << "viewerMode: " << viewerMode << endl;
    
    fontDebugPrint.drawString(tSs.str(), pX, pY); pY += lineHeight;
    
}

void ofApp::drawViewerModeChanger() {
    
    if (viewerMode == 0) {
        ofSetColor(255, 0, 0, 192);
    } else {
        ofSetColor(128, 128, 128, 192);
    }
    //ofRect(0, 700, 200, 50);
    ofRectRounded(ofRectangle(10, myGuiMainMenu.getTop()+5, 180, 40), 10);
    
    ofSetColor(255,255,255,255);
    fontSmall.drawString("Detail", 40, myGuiMainMenu.getTop()+35);
    
    if (viewerMode == 1) {
        ofSetColor(255, 1, 0, 192);
    } else {
        //ofRect(0, 700, 200, 50);
        ofSetColor(128, 128, 128, 192);
    }
    ofRectRounded(ofRectangle(210, myGuiMainMenu.getTop()+5, 180, 40), 10);
    
    ofSetColor(255,255,255,255);
    fontSmall.drawString("List", 240, myGuiMainMenu.getTop()+35);

}

void ofApp::drawModelLRSelector() {
    
    if (myGuiDetailLeftButton.inside(mouseX, mouseY)) {
        ofSetColor(255, 128, 128, 192);
    } else {
        ofSetColor(128, 128, 128, 192);
    }
    ofRectRounded(myGuiDetailLeftButton, 40);
    
    if (myGuiDetailRightButton.inside(mouseX, mouseY)) {
        ofSetColor(255, 128, 128, 192);
    } else {
        ofSetColor(128, 128, 128, 192);
    }
    ofRectRounded(myGuiDetailRightButton, 40);
    
    ofSetColor(255, 255, 255, 255);
    font.drawString("<", myGuiDetailLeftButton.getCenter().x-14, myGuiDetailLeftButton.getCenter().y+14);
    font.drawString(">", myGuiDetailRightButton.getCenter().x-12, myGuiDetailRightButton.getCenter().y+14);

}


void ofApp::drawOpenNi() {
    
    // openNI scanner --------------------------------------------------------------
    
    if (useOpenNi && frameCount >= 1) {  // app start
        //ofShortPixels& depthPixels = oniDepth.getPixelsRef();
        ofMesh oniMesh;
        
        for(int i=0; i<240; i++) {
            
            for(int j=0; j<320; j++) {
                
                ofVec3f worldPos = oniDepth.getWorldCoordinateAt(j, i);
                //worldPos.x *= 10.0;
                //worldPos.y *= 10.0;
                //worldPos.z *= 10.0;
                oniMesh.addColor(ofFloatColor(0.0,1.0,0));
                oniMesh.addVertex(worldPos);
                
            }
        }
        //ofEndShape();
        ofTranslate(0,1000,0);
        ofRotate(-90, 0, 0, 1);
        ofRotate(90, 1, 0, 0);
        ofRotate(90, 0, 1, 0);
        //ofScale(100.0,100.0,100.0);
        glPointSize(2);
        oniMesh.drawVertices();
        //tMesh.drawFaces();
        //tMesh.drawWireframes();
        
    }
    // --------------------------------------------------------------
    
}

void ofApp::resetCam( ) {
    eCam.reset();
    
    if (viewerMode == 0) {
        resetCamDetailView();
    } else {
        resetCamListView();
    }
    
}

void ofApp::resetCamDetailView( ) {
    
    viewerMode = 0;
    
//    eCam.reset();
    
    float modelSizeX = (modelSceneMax[selectMeshId].x - modelSceneMin[selectMeshId].x) *1000;
    float modelSizeY = (modelSceneMax[selectMeshId].y - modelSceneMin[selectMeshId].y) * 1000;
    float modelSizeZ = (modelSceneMax[selectMeshId].z - modelSceneMin[selectMeshId].z) * 1000;
    
    eCam.setPosition(0, -modelSizeZ*1.2, modelSizeZ*1.2);
    eCam.setTarget(ofVec3f(0, 0, 0));
    
}

void ofApp::resetCamListView( ) {
    
    viewerMode = 1;
    
    //eCam.reset();
    eCam.setPosition(3000, -3000, 9000);
    eCam.setTarget(ofVec3f(3000, -3000, 0));
}

void ofApp::detailViewNextModel(int mod) {
    
    selectMeshId = ((selectMeshId - mod) + modelDataNum) % modelDataNum;
    resetCamDetailView();
    
}

void ofApp::drawScaleGrid(float areaSize, int gridSpan) {
    
    areaSize *= 4;
    
    float modelSizeX = areaSize;//modelPosXList[selectMeshId]*1000;
    float modelSizeY = areaSize;//modelHeightList[selectMeshId]*1000;
    float modelSizeZ = areaSize;//modelPosZList[selectMeshId]*1000;
    
    // draw 3axis
    ofSetLineWidth(10);
    ofSetColor(255,64,64);
    ofLine(0,0,0,modelSizeX,0,0);
    //font.drawString("X", 10200, 0);
    ofSetColor(64,192,64);
    ofLine(0,0,0,0,modelSizeY,0);
    //font.drawString("Y", 0, 10200);
    ofSetColor(64,64,255);
    ofLine(0,0,0,0,0,modelSizeZ);
    
    // draw base grid
    //ofSetColor(128,224,255, 64);
    //ofLine(0,0,0,0,0,0);
    //ofRect(0, 0, 10000, 10000);
    
    //ofSetLineWidth(1);
    ofSetLineWidth(2);
    ofSetColor(128,224,255, 128);
    stringstream tSs;
    
    int lineNumX = (areaSize/gridSpan);
    
    for(int j=0; j<lineNumX; j++) {
        int y = j - (lineNumX/2);
        ofLine(-modelSizeX/2, gridSpan*y, 0, modelSizeX/2, gridSpan*y, 0);
        tSs.str("");
        tSs << "" << y*gridSpan/10 << "cm";
        fontSmall.drawString(tSs.str(), -130, gridSpan*y+10);
    }
    
    int lineNumY = (areaSize/gridSpan);
    ofSetColor(128,224,255, 128);
    for(int j=0; j<lineNumY; j++) {
        int x = j - (lineNumY/2);
        ofLine(gridSpan*x, -modelSizeZ/2, 0, gridSpan*x, modelSizeZ/2, 0);
        tSs.str("");
        tSs << "" << x*gridSpan/10 << "cm";
        
        ofPushMatrix();
        ofTranslate(gridSpan*x+10, -30, 0);
        ofRotate(270, 0,0,1);
        fontSmall.drawString(tSs.str(), 0, 0);
        ofPopMatrix();
        
    }
    
}





#pragma mark -  User Interface Functions

// ///////////////////////////////////////////////////////////////////////////////////////////////////
//   Control Functions
// ///////////////////////////////////////////////////////////////////////////////////////////////////


void ofApp::keyPressed(int key){
    
    cout << "keyPressed. key number: " << key << endl;
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
    if (key == ' ') {
        dispAllUiFlag ^= true;
    }
    
    if (key == '0') {
        
        if (viewerMode == 0) {
            resetCamDetailView();
        } else {
            resetCamListView();
        }
        
    } else if (key == '1') {
        
        viewerMode = 0;
        selectMeshId = 0;
        
        resetCamDetailView();
        
    } else if (key == '2') {
        
        viewerMode = 1;
        selectMeshId = 0;
        
        resetCamListView();
        
    } else if (key == '4') {
        
        if (mapNum[0][9] == 0) {
            mapNum[0][9] = 1;
            if (selectMeshId == 0){
                uiEditDisplayFlag = 1;
            }
        } else {
            mapNum[0][9] = 0;
            if (selectMeshId == 0){
                uiEditDisplayFlag = 0;
            }
            
        }
        
    } else if (key == '5') {
        
        if (mapNum[1][9] == 0) {
            mapNum[1][9] = 1;
            if (selectMeshId == 1){
                uiEditDisplayFlag = 1;
            }
        } else {
            mapNum[1][9] = 0;
            if (selectMeshId == 1){
                uiEditDisplayFlag = 0;
            }
        }
        
    } else if (key == '9') {

        uiBtnPlayPause ^= true;
        
    } else if (key == OF_KEY_LEFT) {
        
        if (viewerMode == 0) {
            detailViewNextModel(-1);
        }
        
    } else if (key == OF_KEY_RIGHT) {
        
        if (viewerMode == 0) {
            detailViewNextModel(1);
        }
        
    }
    
    if (key == OF_KEY_TAB) {
        
    }

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
    updateSeekBar(x, y);
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    ofPoint mousePos = ofPoint(x, y);
    
    if (!uiBtnTimerControl) {
        if (myGuiDetailLeftButton.inside(mousePos)) {
            
            if (viewerMode == 0 ) {    // 左右のモデルセレクタ
                
                detailViewNextModel(-1);
                
            }
            
        } else if (myGuiDetailRightButton.inside(mousePos)) {
            
            if (viewerMode == 0 ) {    // 左右のモデルセレクタ
                
                detailViewNextModel(1);
                
            }
        }

    }
    
    updateSeekBar(x, y);

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
    ofPoint mousePos = ofPoint(x, y);
    
    if (uiBtnTimerControl) {
        if (myGuiMainMenu.inside(mousePos)) {

            // view change
            if (x >= 0 && x < 200) {
                viewerMode = 0;
                selectMeshId = 0;
                resetCamDetailView();
            }

            if (x >= 200 && x < 400) {
                viewerMode = 1;
                selectMeshId = 0;
                resetCamListView();
            }
            
            
        }
    }
}

void ofApp::exit() {
    
    //    if (viewerMode = 3) {      // EDIT‚Äû√â¬¢‚Äû√â¬∫‚Äû√â√¢
    // if (uiBtnDebugInfo) {
    cout << "save map file." << endl;
    saveMapFile();
    //}
    //    }
    
    
    //oniDevice->exit();
    //delete oniDevice;
    
    cout << "program exit." << endl;
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
    myGuiSetup();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}

/*
void ofApp::windowResized () {
    
}
 */





#pragma mark -  Utility Functions

// ///////////////////////////////////////////////////////////////////////////////////////////////////
//   Utility Functions
// ///////////////////////////////////////////////////////////////////////////////////////////////////

void ofApp::setPos( ) {
    
    auto posX = position->x;
    auto posY = position->y;
    
}

ofRectangle ofApp::getSubRect( ofRectangle parentRect, ofRectangle subRect ) {
    
    ofRectangle newRect = subRect;
    
    //ofRectangle newRect = ofRectangle(baseRect.x + subRect.x, baseRect.y + subRect.y, subRect.width, subRect.height);
    newRect.translate(parentRect.getPosition());
    
    if (subRect.getRight() > parentRect.getRight()) {
        newRect.width -= subRect.getRight() - parentRect.getRight();
    }
    
    if (subRect.getBottom() > parentRect.getBottom()) {
        newRect.height -= subRect.getBottom() - parentRect.getBottom();
    }
    
    newRect.width = ofClamp(subRect.width, 0, INT_MAX);
    newRect.height = ofClamp(subRect.height, 0, INT_MAX);
    
    return newRect;
}





#pragma mark -  File I/O Functions

// ///////////////////////////////////////////////////////////////////////////////////////////////////
//   File I/O Control Functions
// ///////////////////////////////////////////////////////////////////////////////////////////////////


// --------------------------------------------------------------------------------------------
// Save Map File
// --------------------------------------------------------------------------------------------
void ofApp::saveMapFile() {
    
    ofstream ofs( mapFilePath);
    
    for(int i=0; i<modelDataNum; i++) {
        
        ofs << mapId[i] << ",";
        
        for(int j=0; j<mapDataColumns-1; j++) {
            ofs << int(mapNum[i][j]) << ",";
        }
        
        ofs << mapNum[i][mapDataColumns-1];
        
        ofs << endl;
    }
    
    return;
    
}


// --------------------------------------------------------------------------------------------
// Load Map File
// --------------------------------------------------------------------------------------------
void ofApp::loadMapFile(string meshDataDirPath) {
    
    ///Users/doc100/Desktop/tempData/artdkt_structure3d
    //mapFilePath = "/Users/artdkt_3dscan_20160124_zenhan/artdkt_structure3d/mapFile.csv";
    
    stringstream mapSs;
    mapSs << meshDataDirPath << "/mapFile.csv";
    mapFilePath = mapSs.str();
    
    cout << "mapFilePath: " << mapFilePath;
    
    ofFile mapFile(mapFilePath);
    
    if(!mapFile.exists()){
        
        ofLogError("The file " + mapFilePath + " is missing. I make it.");
        
        for(int i=0; i<256; i++) {
            mapId[i] = "";
            for(int j=0; j<16; j++) {
                mapNum[i][j] = 0;
                if (j == 6) {
                    mapNum[i][j] = 0;
                }
            }
        }
        
    } else {
        
        mapFileExists = true;
        ofBuffer buffer(mapFile);
        
        loadModelDirName = mapFile.getEnclosingDirectory();
        
        // Read file line by line ------------------------------------------------------------------------------
        int bufCounter = 0;
        for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
            string line = *it;
            //Split line into strings
            vector<string> words = ofSplitString(line, ",");
            
            //Store strings into a custom container
            if (words.size()>=2) {
                
                mapId[bufCounter] = words[0];
                
                for(int i=0; i<words.size()-1; i++) {
                    mapNum[bufCounter][i] = stoi(words[i+1]);
                }
            }
            
            cout << line << endl;
            bufCounter++;
        }
        
    }

}


// -------------------------------------------------------------------------------------------------------------
// Read mesh files
// -------------------------------------------------------------------------------------------------------------
void ofApp::dataLoad() {
    
    modeldataLoadingStartTime = ofGetElapsedTimeMillis();
    modeldataFiles = 0;
    
    // 指定したディレクトリ以下のフォルダの名前をdataDirNameListに追加 ---------------------------
    makeDataDirNameListTargetDir(meshDataDirPath);
    
    // 各モデルサイズを記録するための下準備 ----------------------------------
    for(int i=0; i<MAX_MODEL_ARRAY; i++) {
        modelSceneMin[i].x = INT_MAX;
        modelSceneMin[i].y = INT_MAX;
        modelSceneMin[i].z = INT_MAX;
        modelSceneMax[i].x = 0;
        modelSceneMax[i].y = 0;
        modelSceneMax[i].z = 0;
    }
    
    //  --------------------------------------------------------------------
    
    // 各モデルディレクトリ名ごとのループ
    int dirNameLoopCount = 0;
    for (auto dirName : dataDirNameList)
    {
        meshNameList[dirNameLoopCount] = dirName;
        cout << "dirName: " << dirName << endl;
        
        stringstream dirPath;
        dirPath << meshDataDirPath << "/" << dirName << "/";
        
        // 各モデルディレクトリ以下のメッシュファイル数をカウント
        bool staticModelFlag = false;
        int countMeshFileNum = countMeshFileNumTargetDir(dirPath.str());
        if (countMeshFileNum >= 0) {
            maxMeshNumList[dirNameLoopCount] = countMeshFileNum;
        } else {    // 静的モデルの場合
            maxMeshNumList[dirNameLoopCount] = -countMeshFileNum;
            staticModelFlag = true;
        }
        
        // 今回のモデルが動的モデルだった場合の読み込み -----------------------------------------------------------
        if (!staticModelFlag) {

            // スキャンタイムレコードの読み込み　-----------------------------------------------------------------------
            stringstream ss;
            ss.str("");
            ss << dirPath.str() << "scanTimeRecord.csv";
            loadScanTimeRecordFile(ss.str(), dirNameLoopCount);
            
            // メッシュファイルの読み込み ------------------------------------------------------
            ss.str("");
            ss << dirPath.str();
            loadMeshDataTargetDir(ss.str(), dirNameLoopCount);
            
        }

        dirNameLoopCount++;
    }
    modelDataNum = dirNameLoopCount;        // 読み込み対象のモデルデータ数合計

    
    // 読み込んだファイルの総データ容量 -------------------------
    loadFileSizeAll = 0;
    for(auto oneModelList : modelFileSizeList) {
        for(auto fileSize : oneModelFileSizeList) {
            loadFileSizeAll += fileSize;
        }
    }
    
    // 読み込んだメッシュファイルの中で一番大きかったメッシュ数 -------------------------
    totalMaxMeshNum = 0;
    for (auto meshNum : maxMeshNumList) {
        if (meshNum > totalMaxMeshNum) {
            totalMaxMeshNum = meshNum;
        }
    }

    // 全モデルに記録された中で一番大きい（最後の）時間 -------------------------
    totalScanTimeRecordMaxTime = 0;
    for (auto time : scanTimeRecordMaxTime) {
        if (time > totalScanTimeRecordMaxTime) {
            totalScanTimeRecordMaxTime = time;
        }
    }
    
    
    cout<<"modelDataNum:"<<modelDataNum<<endl;
    
    modeldataLoadingEndTime = ofGetElapsedTimeMillis();

}


// 各モデルディレクトリ以下のメッシュファイル数をカウント 静的モデルの場合は-1を返す -----------------
int ofApp::countMeshFileNumTargetDir(string dirPath){
    
    // 各モデルディレクトリ以下を開く
    int meshFileNum = 0;
    
    ofDirectory ofDir;
    ofDir.listDir(dirPath);
    vector<ofFile> files = ofDir.getFiles();
    vector<ofFile>::iterator itr = files.begin();  //
    while( itr != files.end() )  //
    {
        
        if (!itr->isDirectory()) {
            
            string fName = itr->getFileName();
            
            // 動的モデルのメッシュファイル(mesh_XXX.obj)をカウント
            if (fName.substr(0,4) == "mesh" && fName.substr(fName.size()-3,3) == "obj") {
                meshFileNum++;
            }
            
            // 静的モデル (Model.obj)
            if (itr->getFileName().substr(0,9) == "Model.obj") {
                meshFileNum = -1;
                break;
            }
            
        }
        
        ++itr;                 //
    }
    
    cout << "mesh file num: " << meshFileNum << endl;
    return meshFileNum;     // 数え終わったメッシュファイル数を代入

}

// 指定したディレクトリ以下のフォルダの名前をdataDirNameListに追加 ---------------------------
void ofApp::makeDataDirNameListTargetDir(string dirPath) {
    
    ofDirectory ofDir;
    
    ofDir.listDir(dirPath);
    vector<ofFile> files = ofDir.getFiles();
    vector<ofFile>::iterator itr = files.begin();
    
    int tCount = 0;
    while( itr != files.end() )
    {
        
        if (itr->isDirectory()) {
            
            string s = itr->getFileName();
            cout << s << endl;
            dataDirNameList.push_back(s);
            
            if (!mapFileExists) {       // マップファイルが存在する場合
                mapId[tCount] = s;      // 各ディレクトリ名を保存
            }
            
            tCount++;
        }
        ++itr;
    }

}


void ofApp::loadScanTimeRecordFile(string dirPath, int modelIndex) {
    
    int idx = 0;

    struct tm tempTmStruct;
    memset(&tempTmStruct,0x00,sizeof(struct tm));               // Initialize important!
    
    ofFileObj.open(dirPath);
    if (ofFileObj.exists()) {
        
        ofBuffer buffer(ofFileObj);
        
        int targetFrame = (idx*skipLoadFrame);
        int maxFrameOverTest = targetFrame;
        cout << "maxMeshNumList[modelIndex]:" << maxMeshNumList[modelIndex] << endl;
        
        while (!buffer.isLastLine() && (idx < maxLoadMeshNum) && maxFrameOverTest < maxMeshNumList[modelIndex]) {
            
            string line = buffer.getNextLine();
            
            if (line != "") {
                auto itemList = ofSplitString(line, ",");
                //cout << itemList[1] << endl;
                scanTimeRecordList[modelIndex][idx][0] = stoi(itemList[0]);
                scanTimeRecordList[modelIndex][idx][1] = stoi(itemList[1]);
                cout << "scanTimeRecordList[modelIndex][idx][1]: " << scanTimeRecordList[modelIndex][idx][1] << endl;
                
                //itemlist[2] ... "2016/10/09 17:26:56.432"
                string strDateTime = itemList[2].substr(0, 19);           // 2016/10/09 17:26:56
                string strMilliSec = itemList[2].substr(20, 3);           // .432
                
                const char *cStrDateTime = strDateTime.c_str();
                
                ::strptime(cStrDateTime, "%Y/%m/%d %H:%M:%S", &tempTmStruct);
                
                scanTimeRecordTimeStructureList[modelIndex][idx] = mktime(&tempTmStruct);
                scanUnixTimeLongIntList[modelIndex][idx] = ((long)mktime(&tempTmStruct) * 1000) + stoi(strMilliSec);
                
                // min/max keep -----------------
                if (scanUnixTimeModelMinList[modelIndex] >= scanUnixTimeLongIntList[modelIndex][idx]) {
                    scanUnixTimeModelMinList[modelIndex] = scanUnixTimeLongIntList[modelIndex][idx];
                }
                
                if (scanUnixTimeModelMaxList[modelIndex] <= scanUnixTimeLongIntList[modelIndex][idx]) {
                    scanUnixTimeModelMaxList[modelIndex] = scanUnixTimeLongIntList[modelIndex][idx];
                }
                
                if (scanUnixTimeAllItemMin >= scanUnixTimeLongIntList[modelIndex][idx]) {
                    scanUnixTimeAllItemMin = scanUnixTimeLongIntList[modelIndex][idx];
                }
                
                if (scanUnixTimeAllItemMax <= scanUnixTimeLongIntList[modelIndex][idx]) {
                    scanUnixTimeAllItemMax = scanUnixTimeLongIntList[modelIndex][idx];
                }
                // --------------------------
                
                if (itemList.size() >= 10) {
                    for(int x=0; x<7; x++) {
                        scanGpsDataList[modelIndex][idx][x] = stod(itemList[3+x]); // latitude
                    }
                    
                    
                    if (scanGpsDataMaxLat < stod(itemList[3])) {
                        scanGpsDataMaxLat = stod(itemList[3]);
                    }
                    if (scanGpsDataMinLat > stod(itemList[3])) {
                        scanGpsDataMinLat = stod(itemList[3]);
                    }
                    if (scanGpsDataMaxLong < stod(itemList[4])) {
                        scanGpsDataMaxLong = stod(itemList[4]);
                    }
                    if (scanGpsDataMinLong > stod(itemList[4])) {
                        scanGpsDataMinLong = stod(itemList[4]);
                    }
                    
                }
                
                if (itemList.size() >= 20) {
                    cout << "load floatMatrix:";
                    float floatMatrix[16];
                    for(int z2=0; z2 < 16; z2++) {
                        //floatMatrix[z2] = stof(itemList[12+z2]);
                        floatMatrix[z2] = stof(itemList[12+(z2%4*4)+(int)(z2/4)]);
                        cout << floatMatrix[z2] << ", ";
                    }
                    modelMatrixList[modelIndex] = ofMatrix4x4(floatMatrix);
                    cout << endl;
                }
                
                idx++;
                
                //cout << strDateTime << "  ." << strMilliSec << " --- " << scanUnixTimeLongIntList[modelIndex][idx] << endl;
            }
            
            for(int p=0; p<(skipLoadFrame-1); p++) {
                line = buffer.getNextLine();
            }
            
        }
        
        scanTimeRecordMaxTime[modelIndex] = scanTimeRecordList[modelIndex][idx-1][1];
        cout << "scanTimeRecordMaxTime: " << scanTimeRecordMaxTime[modelIndex] << endl;
        
        ofFileObj.close();
        
    } // scanTimeRecordの読み込み終了
    
    if (idx > maxLoadedMeshNumInAllMesh) {  // 数え終わったメッシュ数を保存
        maxLoadedMeshNumInAllMesh = idx;
    }
    
}

    // メッシュファイルの読み込み ------------------------------------------------------
void ofApp::loadMeshDataTargetDir(string dirPath, int modelIndex) {
    
    stringstream ss;
    oneModelFileSizeList.clear();

    for(int i=0; (i*skipLoadFrame)<maxMeshNumList[modelIndex]; i++) {
        
        // 各メッシュファイルパスの取得 --------
        ss.str("");
        ss << dirPath << "mesh_" << ((i*skipLoadFrame)+startPlayMeshAnimNum) << ".obj";
        //ss << "F:/ArtDKT_kuwakubo_3dscan_20160123to25/artdkt_3dscan_20160124_kouhan/artdkt_structure3d/38/mesh_" << (i+2) << ".obj";
        string objFilePath = ss.str();
        
        // メッシュファイルのオープン --------
        ofFileObj.open(objFilePath);
        if (!ofFileObj.exists()) {          // メッシュファイルが存在しない場合
            
            cout << objFilePath << " file not found." << endl;
            
        } else {         // メッシュファイルが存在する場合
            
            oneModelFileSizeList.push_back(ofFileObj.getSize()); // getFileSize
            
            ofFileObj.close();
            
            // add
            ss.str("");
            //ss << dirPath.str() << "mesh_" << ((i*skipLoadFrame)+2+startPlayMeshAnimNum) << ".jpg";
            ss << objFilePath.substr(0,objFilePath.size()-4) << ".jpg";
            string objImageFilePath = ss.str();
            if (loadPictureMode) {
                modelImageList[modelIndex][i].loadImage(objImageFilePath);
            }
            
            //Assimp ver.
            
            asModelObj[modelIndex][i].loadModel(objFilePath );
            
            // 各メッシュの頂点数の保存 ---------------
            if (asModelObj[modelIndex][i].getMeshCount()) {
                ofMesh tMesh = asModelObj[modelIndex][i].getMesh(0);
                meshVertexNumList[modelIndex][i] = tMesh.getVertices().size();
            }
            
            // モデルの物理サイズの計算と保存 -----------
            ofPoint tMin = asModelObj[modelIndex][i].getSceneMin();
            ofPoint tMax = asModelObj[modelIndex][i].getSceneMax();
            cout << "tMin: " << tMin << " tMax: " << tMax << endl;
            
            if (tMin.x == 0 && tMin.y == 0 && tMin.z == 0 && tMax.x == 0 && tMax.y == 0 && tMax.z == 0) {
                cout << "======---------- Hit! ----------------==============================================" << endl;
            } else  {
                
                if (tMin.x < modelSceneMin[modelIndex].x) {
                    modelSceneMin[modelIndex].x = tMin.x;
                }
                if (tMin.y < modelSceneMin[modelIndex].y) {
                    modelSceneMin[modelIndex].y = tMin.y;
                }
                if (tMin.z < modelSceneMin[modelIndex].z) {
                    modelSceneMin[modelIndex].z = tMin.z;
                }
                if (tMax.x > modelSceneMax[modelIndex].x) {
                    modelSceneMax[modelIndex].x = tMax.x;
                }
                if (tMax.y > modelSceneMax[modelIndex].y) {
                    modelSceneMax[modelIndex].y = tMax.y;
                }
                if (tMax.z > modelSceneMax[modelIndex].z) {
                    modelSceneMax[modelIndex].z = tMax.z;
                }
            }
            
        }
        
        cout << "modelSceneMin[" << modelIndex << "]: " << modelSceneMin[modelIndex] << endl;
        cout << "modelSceneMax[" << modelIndex << "]: " << modelSceneMax[modelIndex] << endl;
        
        cout << "file load: " << ss.str() << endl;
    }
    
    modelFileSizeList.push_back(oneModelFileSizeList);
    
    modeldataFiles += maxMeshNumList[modelIndex];

}
