#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    // 全力再生
    ofSetVerticalSync(false);
    ofSetFrameRate(0);
    
    appInitStartTime = ofGetElapsedTimeMillis();
    
    defaultMeshDataDirPath = "/Users/artdkt/Desktop/3dscan_data_for0630/artdkt_structure3d";

    // setting vars --------------------------------------------

    useOpenNi = false;
    dualColorSystem = true;           // 同じモデルデータを２つのライブラリで同時に読み込み、切り替えながら表示します。２倍動作に時間がかかり、メモリ消費も２倍です。
    loadPictureMode = false;
    loadVertexColorObj = false;         // trueにすると頂点カラー対応（テクスチャ非対応）のライブラリ用にモデルを別に読み込みます　メモリを大量に消費します。
    colorMode = 1;

    viewerMode = 1;
    //uiMeshDrawType = 1;
    selectMeshId = 0;
    mapFileExists = false;
    mapDataColumns = 16;
    
    totalLoadedModelNum = 0;
    maxLoadedMeshNumInAllMesh = 0;
    
    
    //maxLoadMeshNum = 6;
    //skipLoadFrame = 15;
    maxLoadMeshNum = 2000;
    skipLoadFrame = 10;
    
    playMode = 0;   // 1:timebased 0: frame

    // initializing vars ---------------------------------------
    
    frameCount = 0;
    playCount = 0;
    totalMaxMeshNum = 0;
    startPlayMeshAnimNum = 0;
    
    nowPlayTime = 0;
    
    dispGui = true;
    dispDebugInfoFlag = true;
    dispAllUiFlag = true;
    dispPlayControlUiFlag = true;
    
    loopPlay = true;
    
    prevSelectModel = -1;

    playSeekTime = 0;
    onPlay = true;
    
    playStartPrevPos = 0;
    prevFramePlayState = true;
    
    seekbarAddTime = 0;

    scanUnixTimeAllItemMin = LONG_MAX;
    scanUnixTimeAllItemMax = LONG_MIN;
    scanGpsDataMinLat = 1000;
    scanGpsDataMinLong = 1000;
    scanGpsDataMaxLat = 0;
    scanGpsDataMaxLong = 0;
    
    fboFront.allocate(640, 480, GL_RGBA);
    fboSide.allocate(640, 480, GL_RGBA);
    fboTop.allocate(640, 480, GL_RGBA);
    fboCam.allocate(640, 480, GL_RGBA);
    
    // get file load path --------------------------------------------------------------------------------
    
    //meshDataDirPath = "/Users/artdkt_3dscan_20160124_zenhan/artdkt_structure3d";
    //meshDataDirPath = "/Users/doc100/Desktop/tempData/artdkt_structure3d";
    
    ofFileDialogResult fileDialogResult = ofSystemLoadDialog("Select Time-based 3D Record base directory.", true, defaultMeshDataDirPath);
    
    meshDataDirPath = fileDialogResult.getPath();
    
    // Read Map File ----------------------------------------------------------------
    loadMapFile(meshDataDirPath);
    
    // load fonts --------------------------------------------------------------
    
    font.loadFont("hira.otf", 38);
    fontSmall.loadFont("hira.otf", 20);
    fontLarge.loadFont("hira.otf", 100);
    fontDebugPrint.loadFont("hira.otf", 12);
    ofxGuiSetFont("hira.otf", 11, true, true);
    
    ofxGuiSetTextPadding(3);
    ofxGuiSetDefaultWidth(300);     // ウィンドウ幅決め？
    
    // myGuiSetup ----------------------------------------------------------------------
    
    myGuiSetup();
    
    // OfxGUI setup -----------------------------------------------------------------
    
    ofColor initColor = ofColor(0, 127, 255, 255);
    ofColor minColor = ofColor(0,0,0,0);
    ofColor maxColor = ofColor(255,255,255,255);
 
    ofVec2f initPos = ofVec2f(ofGetWidth(), ofGetHeight());
    ofVec2f minPos = ofVec2f(-ofGetWidth() * 3, -ofGetHeight() * 4);
    ofVec2f maxPos = ofVec2f(ofGetWidth() * 2, ofGetHeight() * 3);
   
    uiBtnPlayPause.setSize(100, 100);
    

    // Set GUI parts -------------------------------------------------------------------------------
    guiPlayControl.setup("PlayControl");
    guiPlayControl.setPosition(0, ofGetHeight()-100);
    guiPlayControl.setDefaultHeight(24);
    
    gui.setup("settings");
    //gui.setFillColor(<#const ofColor &color#>)
    
    gui.setDefaultBackgroundColor(ofColor(255,0,0));
    
    gui.setPosition(0, 0);
    gui.setDefaultHeight(24);
    gui.setBackgroundColor(ofColor(0,0,0,32));
    gui.setShowHeader(false);

    gui.add(uiThumbnailIconDistance.setup("thumbnailIconDistance", 0, 0, 5000));
    gui.add(uiIconNumX.setup("iconNumX", 4, 1, 8));
    gui.add(uiMeshDrawType.setup("meshDrawType", 0, 0, 2));
    gui.add(uiColorMode.setup("colorMode", 1, 0, 1));
    gui.add(uiPlayMode.setup("playMode", 2, 0, 2));
    gui.add(uiFramerate.setup("framerate", 60, 5, 60));
    gui.add(uiBtnLight.setup("Light on/off", true, 40, 40));
    gui.add(uiBtnGrid.setup("Grid", false, 40, 40));
    gui.add(uiBtnDebugInfo.setup("DebugInfo", false, 40, 40));
    gui.add(uiBtnTurnMesh.setup("TurnMesh", true, 40, 40));
    gui.add(uiBtnLoopPlay.setup("LoopPlay", true, 40, 40));
    gui.add(uiBtnOrtho.setup("Ortho", false, 40, 40));
    gui.add(uiBtnTimerControl.setup("TimerControl", false, 40, 40));
    gui.add(uiGpsMapMode.setup("mapMode", 0, 0, 3));
    gui.add(uiTestSlider.setup("TestSlider", 0 ,  -10000, 10000));
    gui.add(uiBtnReset.setup("Reset", 40, 40));
    gui.add(uiBtnSelectReset.setup("quit", 40, 40));
    
    
    guiMapEdit.setDefaultWidth(300);
    guiMapEdit.setup("MapEdit");
    gui.setDefaultHeight(24);
    guiMapEdit.setPosition(0, 0);
    guiMapEdit.setWidthElements(300);
    guiMapEdit.setShowHeader(false);

    
    guiMapEdit.setDefaultBackgroundColor(ofColor(255,0,0));
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
    
    prevPosX = position->x;
    prevPosY = position->y;
    
    uiBtnReset.addListener(this, &ofApp::resetCam);
    
    guiPlayItem.setup("playItem");
    
    guiPlayItem.add(uiBtnPlaySelectA.setup("A", 50, 40));
    guiPlayItem.add(uiBtnPlaySelectB.setup("B", 50, 40));
    guiPlayItem.add(uiBtnPlaySelectC.setup("C", 50, 40));
    guiPlayItem.add(uiBtnPlaySelectBack.setup("Reset", 50, 40));
    
    
    guiPage.setup("single page");
    guiPage.setSize(300, 300);
    guiPage.add(&gui);
    
    guiPage2.setup("page 1");
    guiPage2.add(&guiMapEdit);
    //guiPage2.add(&rotary);
    
    guiTabbedPages.setup("tabbed pages", "", 100);
    guiTabbedPages.setSize(300, 700);
    guiTabbedPages.setTabWidth(70);
    guiTabbedPages.setTabHeight(30);
    guiTabbedPages.add(&guiPage);
    guiTabbedPages.add(&guiPage2);
    
    // OpenNI ----------------------------------------------
    
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
        
        //	if (color.setup(*device)) // only for kinect device
        //	{
        //		color.setSize(320, 240);
        //		color.setFps(60);
        //		color.start();
        //	}

    }
    
    // ------------------------------------------------
    
    appInitEndTime = ofGetElapsedTimeMillis();
    playStartDateTime = ofGetElapsedTimeMillis();

    uiPlayMode = playMode;
 
    ::time(&unixTimeOnOfStarted);

}

// setup end ----------------------------------------------------------------------------------

void ofApp::myGuiSetup() {

    guiPlayControl.setPosition(0, ofGetHeight()-100);
    
    myGuiMain = ofRectangle(0, ofGetHeight()-100, ofGetWidth(), 100);
    mainView = ofRectangle(0, 0, ofGetWidth(),ofGetHeight() - myGuiMain.height);
    
    myGuiMainMenuDiff = ofRectangle(0, 50, ofGetWidth(), 50);
    myGuiMainTimebarDiff = ofRectangle(0, 0, ofGetWidth(), 50);
    
    myGuiMainMenu = myGuiMain;
    myGuiMainMenu.y += 50;
    
    myGuiMainTimebar = myGuiMain;
    myGuiMainTimebar.setHeight(50);
    
    myGuiDetailLeftButton = getSubRect( mainView, ofRectangle(0, 100, 100, ofGetHeight()-100-myGuiMain.getHeight() ) );
    
    myGuiDetailRightButton = getSubRect( mainView, ofRectangle(ofGetWidth()-100, 100, 100, ofGetHeight()-100-myGuiMain.getHeight() ) );
    
    myGuiDispGuiToggle = getSubRect( myGuiMainMenu, ofRectangle(ofGetWidth()-50, ofGetHeight()-50, 50, 50) );
    
    
    myGuiPlayButton = getSubRect( myGuiMainMenu, ofRectangle(0,0,50,50) );

}



// ///////////////////////////////////////////////////////////////////////////////////////////////////
//   Update Functions
// ///////////////////////////////////////////////////////////////////////////////////////////////////

void ofApp::update(){
    
    int i = uiEditModelSelector;
    
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
    
    if (uiBtnTimerControl) {
        eCam.disableMouseInput();
    } else {
        eCam.enableMouseInput();
    }
    
    if (prevFramePlayState == false && uiBtnPlayPause) {
        playStartPrevPos = nowPlayTime;
        playStartDateTime = ofGetElapsedTimeMillis();
    }
    
}


// ///////////////////////////////////////////////////////////////////////////////////////////////////
//   Draw Functions
// ///////////////////////////////////////////////////////////////////////////////////////////////////

void ofApp::draw(){
    
    ofSetFrameRate(uiFramerate);

    displayTotalVertices = 0;
    
    if (frameCount == 1) {  // app start
        dataLoad();
    }
    
    // --------------------------
    ofBackground(240, 240, 240);        // gray bg
    
    
    
    ofSetColor(0, 0, 0, 32);
    ofDrawRectangle(myGuiMain);//0,ofGetHeight()-100, ofGetWidth(), 100);
    
    ofEnableSmoothing();
    
    if(frameCount==1){
        eCam.begin();
        eCam.reset();
        eCam.setPosition(0,0,5000);
        eCam.end();
        
    }
    
    //  ---------------------------------------
    ofSetColor(255,255,255,255);
    
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
    
    eCam.setFarClip( 100000.0f );
    
    ofSetColor(255,255,255,255);
    
    if (frameCount == 0) {
        font.drawString("Now Loading...", 500, 400);
    }
    
    
    eCam.begin();
    if (uiBtnOrtho) {
        eCam.enableOrtho();
    } else {
        eCam.disableOrtho();
    }
    
    ofScale(1,-1);      // y-axis reverse! (for fix drawString text flips promblem)

    glPushMatrix();

    if (uiBtnGrid) {
        ofSetColor(255,255,255,255);
        ofDrawGrid(uiThumbnailIconDistance);
    }
    
    //glTranslatef(0, 0, 0); //
    
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
            
            if (uiPlayMode == 2) {  // Datetime-based sync play
                
                if (maxMeshNumList[i] >= 2 && scanTimeRecordMaxTime[i] > 0) {
                    
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
                        modelFlagList[i] = 2;
                    }

                    
                }
            }
            
            else if (uiPlayMode == 1) {
                
                // get play frame from play time ------------------------------------------------------------------
                if (maxMeshNumList[i] >= 2 && scanTimeRecordMaxTime[i] > 0) {       // error kaihi
                    
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
                
            } else {        // uiPlayMode == 0  frame base play
                
                playFrameSelector = counter;
                
            }
            
        }
        
        if (viewerMode == 0) {
            
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

            asModelObj[i][playFrameSelector].draw(OF_MESH_FILL);
            
            /*
            float modelSizeX = modelPosXList[selectMeshId]*1000;
            float modelSizeY = modelHeightList[selectMeshId]*1000;
            float modelSizeZ = modelPosZList[selectMeshId]*1000;
            */
            
            if (i == selectMeshId) {
                float modelSize = modelSceneMax[i].x - modelSceneMin[i].x;
                drawScaleGrid(modelSize*1000, 100);
            }
            
            displayTotalVertices += modelList[i][playFrameSelector].getNumVertices();
            glPushMatrix();  //
            
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
            
            
            if (uiBtnTurnMesh) {
                glRotatef(-90, 1, 0, 0);
                
            }             
            glTranslatef(mapNum[i][0], mapNum[i][1], mapNum[i][2]);

            
            if (uiColorMode == 0) {
                //ofTranslate(0,0,1500);      // goto center
                //ofTranslate(0,-1*modelHeightList[i]*1000,0);    // set y pos
                //ofTranslate(0,-0,modelPosZList[i]*1000);        // hosei
            
                ofScale(1000, 1000, 1000);
            }
            ofScale(1, 1, -1);      // fix model direction
            if (uiColorMode == 1) {
                asModelObj[i][playFrameSelector].setScaleNormalization(false);
                ofScale(-1, -1, 1);      // fix model direction
                //ofTranslate(0,-0,modelPosZList[i]*1000);        // hosei
//                ofTranslate(1500,1100,-2500);      // goto center
                
                ofScale(1000, 1000, 1000);
            }
            ofRotateX(mapNum[i][3]);
            ofRotateY(mapNum[i][4]);
            ofRotateZ(mapNum[i][5]);
            
            double centerX = modelSceneMin[i].x + (modelSceneMax[i].x - modelSceneMin[i].x) / 2;
            double centerY = modelSceneMin[i].y + (modelSceneMax[i].x - modelSceneMin[i].x) / 2;
            double centerZ = modelSceneMin[i].z + (modelSceneMax[i].x - modelSceneMin[i].x) / 2;
            ofTranslate(centerX, centerY, -centerZ);
            
            if (mapNum[i][7] >= 1) {
                glRotatef(180, 0, 1, 0);
                ofTranslate(0,0,0);
            }
            if (mapNum[i][8] >= 1) {
                glRotatef(180, 1, 0, 0);
                //ofTranslate(0,0,-530);
            }
            if (mapNum[i][6] >= 1) {
                glRotatef(180, 0, 0, 1);
                //ofTranslate(0,0,-530);
            }
            
            
            if (mapNum[i][9] == 0) {
                if (dualColorSystem == true && uiColorMode == 1) {
                    
                    if (uiMeshDrawType == 1) {
                        ofSetLineWidth(1);
                        asModelObj[i][playFrameSelector].draw(OF_MESH_WIREFRAME);
                    } else if (uiMeshDrawType == 2) {
                        
                        glPointSize(4);
                        //ofBlendMode(OF_BLENDMODE_ALPHA);

                        asModelObj[i][playFrameSelector].draw(OF_MESH_POINTS);
                    } else {
                        //asModelObj[i][counter].drawFaces();
                        asModelObj[i][playFrameSelector].draw(OF_MESH_FILL);
                    }
                    
                } else {
                    if (uiMeshDrawType == 1) {
                        ofSetLineWidth(1);
                        modelList[i][playFrameSelector].drawWireframe();
                    } else if (uiMeshDrawType == 2) {
                        glPointSize(5);
                        modelList[i][playFrameSelector].drawVertices();
                    } else {
                        modelList[i][playFrameSelector].draw();
                    }
                }
            }
            
            glPopMatrix();
            
            glPushMatrix();
            {
                glRotatef(90, 1, 0, 0);
                ofTranslate(0,0,1000);
                ofScale(10,10,10);
                modelImageList[i][playFrameSelector].draw(0,0);
            }
            glPopMatrix();

            
        }
        else if (viewerMode <= 1) {
            
            if (i == 0) {
                drawScaleGrid(10000, 1000);
            }
            
            if (uiGpsMapMode == 0) {
                
                displayTotalVertices += modelList[i][playFrameSelector].getNumVertices();
                
                glPushMatrix();
                
                
                // リスト配置分
                glTranslatef(((i%uiIconNumX))*uiThumbnailIconDistance, (i/uiIconNumX)*uiThumbnailIconDistance, 0); //√Å√Æ¬™√à√π¬¢‚Ä∞‚àè‚â†√Ç√∏√â‚Äû√Ö¬¥√Å√ü¬™√Ç√£√Ø
                
                //glRotatef(180, 0, 1, 0);        //

                int gridSize = 1000;
                int gridDiv = 10;
                int gridRowSize = gridSize / gridDiv;
                
                // Draw Model Name
                {
                    glPushMatrix();  //
                    ofSetColor(255,255,255,255);
                    //glTranslatef(180, 400, 700); //
                    glTranslatef(-0,0,2 );

                    ofDisableLighting();        //
                    ofSetColor(0,0,0);
                    fontLarge.drawString(dataDirNameList[i],0,0);        // display model name
                    ofEnableLighting();

                    //font.drawString(to_string(asModelObj[i][counter].getNumMeshes()),600,500);
                    glPopMatrix();
                }
                
                
                if (uiBtnTurnMesh) {
                    glRotatef(-90, 1, 0, 0);
                    
                }
                
                if (mapNum[i][7] >= 1) {
                    glRotatef(180, 0, 1, 0);
                    ofTranslate(0,0,-530);
                }
                if (mapNum[i][8] >= 1) {
                    glRotatef(180, 1, 0, 0);
                    //ofTranslate(0,0,-530);
                }
                if (mapNum[i][6] >= 1) {
                    glRotatef(180, 0, 0, 1);
                    //ofTranslate(0,0,-530);
                }
                ofRotateX(mapNum[i][3]);
                ofRotateY(mapNum[i][4]);
                ofRotateZ(mapNum[i][5]);
                
                
                if (uiColorMode == 0) {
                    //ofTranslate(0,0,1500);      // goto center
                    //ofTranslate(0,-1*modelHeightList[i]*1000,0);    // set y pos
                    //ofTranslate(0,-0,modelPosZList[i]*1000);        // hosei
                    
                    ofScale(1000, 1000, 1000);
                }
                ofScale(1, 1, -1);      // fix model direction
                if (uiColorMode == 1) {
                    asModelObj[i][playFrameSelector].setScaleNormalization(false);
                    ofScale(-1, -1, 1);      // fix model direction
                    //ofTranslate(0,-0,modelPosZList[i]*1000);        // hosei
                    //                ofTranslate(1500,1100,-2500);      // goto center
                    
                    ofScale(1000, 1000, 1000);
                }
                
                
                // ---------------------------------------------------
                if (modelFlagList[i] == 0) {            // not effect vertex color object
                    ofSetColor(255, 255, 255, 255);
                } else if (modelFlagList[i] == 1) {
                    ofSetColor(255, 255, 255, 32);
                } else if (modelFlagList[i] == 2) {
                    ofSetColor(0, 255, 0, 64);
                }
                
                if (mapNum[i][9] == 0) {
                    
                    
                    glTranslatef(mapNum[i][0], mapNum[i][1], mapNum[i][2]);
                    
                    double centerX = modelSceneMin[i].x + (modelSceneMax[i].x - modelSceneMin[i].x) / 2;
                    double centerY = modelSceneMin[i].y + (modelSceneMax[i].x - modelSceneMin[i].x) / 2;
                    double centerZ = modelSceneMin[i].z + (modelSceneMax[i].x - modelSceneMin[i].x) / 2;
                    ofTranslate(centerX, centerY, -centerZ);
                    
                    if (dualColorSystem == true && uiColorMode == 1) {
                        
                        if (uiMeshDrawType == 1) {
                            ofSetLineWidth(1);
                            asModelObj[i][playFrameSelector].draw(OF_MESH_WIREFRAME);
                        } else if (uiMeshDrawType == 2) {
                            asModelObj[i][playFrameSelector].draw(OF_MESH_POINTS);
                        } else {
                            //asModelObj[i][counter].drawFaces();
                            asModelObj[i][playFrameSelector].draw(OF_MESH_FILL);
                        }
                        
                    } else {

                        if (uiMeshDrawType == 1) {
                            //asModelObj[i][counter].draw(OF_MESH_WIREFRAME);
                            //            asModelObj[i][counter].drawWireframe();
                            ofSetLineWidth(1);
                            
                            modelList[i][playFrameSelector].drawWireframe();
                        } else if (uiMeshDrawType == 2) {
                            //asModelObj[i][counter].draw(OF_MESH_POINTS);
                            //asModelObj[i][counter].draw(OF_MESH_POINTS);
                            modelList[i][playFrameSelector].drawVertices();
                        } else {
                            
                            modelList[i][playFrameSelector].draw();
                            
                            //asModelObj[i][counter].drawFaces();
                            //asModelObj[i][counter].draw(OF_MESH_FILL);
                        }
                    }
                }
            } else if (uiGpsMapMode == 1) {
                
                //cout << "maxMeshNumList" << maxMeshNumList[i] << endl;
                
                for(int z=0; z<maxMeshNumList[i]; z++) {
                    displayTotalVertices += modelList[i][z].getNumVertices();

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
                        
                        if (dualColorSystem == true && uiColorMode == 1) {
                            
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
                            
                        } else {

                            if (uiMeshDrawType == 1) {
                                //asModelObj[i][counter].draw(OF_MESH_WIREFRAME);
                                //            asModelObj[i][counter].drawWireframe();
                                ofSetLineWidth(1);
                                
                                modelList[i][playFrameSelector].drawWireframe();
                            } else if (uiMeshDrawType == 2) {
                                //asModelObj[i][counter].draw(OF_MESH_POINTS);
                                //asModelObj[i][counter].draw(OF_MESH_POINTS);
                                glPointSize(5);

                                modelList[i][playFrameSelector].drawVertices();
                            } else {
                                
                                modelList[i][playFrameSelector].draw();
                                
                                //asModelObj[i][counter].drawFaces();
                                //asModelObj[i][counter].draw(OF_MESH_FILL);
                            }
                        }
                    }
                    glPopMatrix();
                    
                }

            } else if (uiGpsMapMode == 3) {
                
                //cout << "maxMeshNumList" << maxMeshNumList[i] << endl;
                
                for(int z=0; z<maxMeshNumList[i]; z++) {
                    displayTotalVertices += modelList[i][z].getNumVertices();
                    
                    glPushMatrix();
                    
                    playFrameSelector = z;
                    
                    ofVec3f tr = modelMatrixList[z].getTranslation();
                    double posX = tr.x*uiTestSlider;//;(scanGpsDataList[i][z][1] - scanGpsDataMinLong) * longScale;
                    double posY = tr.z*uiTestSlider; //;(scanGpsDataList[i][z][0] - scanGpsDataMinLat) * latScale;
                    
                    cout << "posX: " << posX << " posY: " << posY << endl;
                    
                    
                    if (uiBtnTurnMesh) {
                        glRotatef(-90, 1, 0, 0);
                        
                    }
                    //ofTranslate(0,-1*modelHeightList[i]*1000,0);
                    //ofTranslate(0,-0,modelPosZList[i]*1000);        // hosei
                    
                    // 6/29
                    ofScale(1, 1, -1);      // fix model direction
                    if (uiColorMode == 1) {
                        ofScale(-1, -1, 1);      // fix model direction
                    }
                    
                    ofTranslate(0,0,posY);
                    ofTranslate(posX,0,0);
                    
                    ofScale(1000, 1000, 1000);  // temp debug
                    
                    if (modelFlagList[i] == 0) {            // not effect vertex color object
                        ofSetColor(255, 255, 255, 255);
                    } else if (modelFlagList[i] == 1) {
                        ofSetColor(255, 255, 255, 32);
                    } else if (modelFlagList[i] == 2) {
                        ofSetColor(0, 255, 0, 64);
                    }
                    ofSetColor(255, 255, 255, 255);
                    
                    if (mapNum[i][9] == 0) {
                        
                        if (dualColorSystem == true && uiColorMode == 1) {
                            
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
                            
                        } else {
                            
                            if (uiMeshDrawType == 1) {
                                //asModelObj[i][counter].draw(OF_MESH_WIREFRAME);
                                //            asModelObj[i][counter].drawWireframe();
                                ofSetLineWidth(1);
                                
                                modelList[i][playFrameSelector].drawWireframe();
                            } else if (uiMeshDrawType == 2) {
                                //asModelObj[i][counter].draw(OF_MESH_POINTS);
                                //asModelObj[i][counter].draw(OF_MESH_POINTS);
                                glPointSize(5);
                                
                                modelList[i][playFrameSelector].drawVertices();
                            } else {
                                
                                modelList[i][playFrameSelector].draw();
                                
                                //asModelObj[i][counter].drawFaces();
                                //asModelObj[i][counter].draw(OF_MESH_FILL);
                            }
                        }
                    }
                    glPopMatrix();
                    
                }
                
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
            
            } else {        // GPS Walk thru mode
                
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
                        
                        if (uiMeshDrawType == 1) {
                            //asModelObj[i][counter].draw(OF_MESH_WIREFRAME);
                            //            asModelObj[i][counter].drawWireframe();
                            ofSetLineWidth(1);
                            
                            modelList[i][playFrameSelector].drawWireframe();
                        } else if (uiMeshDrawType == 2) {
                            //asModelObj[i][counter].draw(OF_MESH_POINTS);
                            //asModelObj[i][counter].draw(OF_MESH_POINTS);
                            modelList[i][playFrameSelector].drawVertices();
                        } else {
                            
                            modelList[i][playFrameSelector].draw();
                            
                            //asModelObj[i][counter].drawFaces();
                            //asModelObj[i][counter].draw(OF_MESH_FILL);
                        }
                    }
                    glPopMatrix();

                }
                glPopMatrix();
                    
            }

            ofSetColor(255, 255, 255, 255);
            
            glPopMatrix();
            
        } else  {           // mapView -----------------------------------------------------------
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
            
            glPopMatrix();  //√ã¬Æ√≤√ä√ú‚àÇ‚Äû√Ö√≥‚Äû√Ö√º‚Ä∞Œ©√ß√ÅŒ©√Ü‚Äû√Ö¬¥√ä√†¬™‚Äû√Ö√¥

        }
        
    }       // for loop
    
    
    
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
    

    
    glPopMatrix();  //√ã¬Æ√≤√ä√ú‚àÇ‚Äû√Ö√≥‚Äû√Ö√º‚Ä∞Œ©√ß√ÅŒ©√Ü‚Äû√Ö¬¥√ä√†¬™‚Äû√Ö√¥
    
    eCam.end();
    
    if (viewerMode == 0) {
        
        glPushMatrix();
        //glRotatef(90, 1, 0, 0);
        /*
        ofTranslate(0,0,0);
        ofScale(10,10,10);
         */
        fboCam.draw(0,160);
        ofScale(0.5,0.5,0.5);
        modelImageList[selectMeshId][playFrameSelector].draw(0,0);
        glPopMatrix();
        
    }
    
    if (uiBtnLight) {
        
        ofDisableLighting();        // ‚Äû√Ö√¨‚Äû√á√•‚Äû√á√≠‚Äû√Ö√≥‚Äû√Ö‚Ñ¢‚Äû√Ö√ë‚Äû√Ö¬ÆUI‚Äû√Ö‚Ñ¢‚Äû√Ö¬©‚Äû√Ö√Ü√ã√¢‚â§‚Äû√Ö√•‚Äû√Ö√§‚Äû√Ö√£‚Äû√Ö√≥‚Äû√Ö√®‚Äû√Ö‚Ñ¢‚Äû√á√£
        light.disable();
        ofDisableDepthTest();
    }

    
    
    // UI ---------------------------------------------------
    stringstream tSs;
    
    int pX = 40;
    int pY = 80;
    int fSize = 10;
    int lineHeight = fSize*2;
    if (dispAllUiFlag) {
    
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
        tSs << "maxLoadedModelNum: " << maxLoadedMeshNumInAllMesh;
        fontDebugPrint.drawString(tSs.str(), pX, pY); pY += lineHeight;

        if (viewerMode == 0) {
            tSs.str("");
            tSs << "meshName: " << meshNameList[selectMeshId];
            fontSmall.drawString(tSs.str(), pX, pY); pY += lineHeight;
        }

        ofSetColor(255,255,255,255);
        
        if (uiBtnDebugInfo) {
            stringstream tSs;
            tSs.str("");
            tSs << "mouseX: " << mouseX << " mouseY: " << mouseY << "eCam.x: " << eCam.getX() << " eCam.y: " << eCam.getY() << " eCam.z" << eCam.getZ();
            
            fontDebugPrint.drawString(tSs.str(), 50, 700);

            
            ofVec3f worldPos = eCam.screenToWorld(ofVec3f(mouseX, mouseY, 0));
            stringstream tSs2;
            tSs2.str("");
            tSs2 << "worldX: " << worldPos.x << " worldY: " << worldPos.y << " worldZ: " << worldPos.z;
            fontDebugPrint.drawString(tSs2.str(), 50, 750);

            stringstream tSs3;
            
            tSs3 << "viewerMode: " << viewerMode << endl;
            
            fontSmall.drawString(tSs3.str(), 50, 800);
        }

        /*
        if (mouseX >= gui.getPosition().x
            && mouseX < (gui.getPosition().x + gui.getWidth())
            && mouseY >= gui.getPosition().y
            && mouseY < (gui.getPosition().y + gui.getHeight()) ) {
            eCam.disableMouseInput();
        } else {
            eCam.enableMouseInput();
        }
        */
        
        auto posX = position->x;
        auto posY = position->y;
        if (prevPosX != posX || prevPosY != posY) {
            //eCam.setPosition(posX, posY, eCam.getZ());
        }
        
        
        int barWidth = ofGetWidth()*10/10;
        int barX = 0;//ofGetWidth() / 10;
        int progressPosX;
        
        if (frameCount>1) {
            if (uiPlayMode == 2) {
                progressPosX = (nowPlayTime  * barWidth ) / (scanUnixTimeAllItemMax - scanUnixTimeAllItemMin);
            } else if (uiPlayMode == 1) {
                progressPosX = (nowPlayTime  * barWidth ) / totalScanTimeRecordMaxTime;
            } else {
                
                if (viewerMode == 0) {
                    progressPosX = (playCount * barWidth ) / maxMeshNumList[selectMeshId];
                } else if (viewerMode == 1) {
                    progressPosX = (playCount * barWidth ) / maxLoadedMeshNumInAllMesh;
                }
            }
        }
        
        ofSetColor(64,64,64, 64);
        ofSetLineWidth(3);
        ofDrawLine(barX, ofGetHeight() - 130, barX+barWidth, ofGetHeight() - 130);
        ofDrawCircle(progressPosX+barX+2, ofGetHeight() - 130+2, 24);
        ofSetColor(255,255,255);
        ofDrawCircle(progressPosX+barX, ofGetHeight() - 130, 20);

        tSs.str("");
        tSs << "nowPlayTime: " << nowPlayTime << "    /  endTime: " << totalScanTimeRecordMaxTime;
        fontSmall.drawString(tSs.str(), pX, ofGetHeight() - 230);

        tSs.str("");
        if (viewerMode == 1) {
            tSs << "playCount: " << playCount << "   /  endMeshNum: " << totalMaxMeshNum;
        } else if (viewerMode == 0) {
            tSs << "playCount: " << playCount << "   /  endMeshNum: " << maxMeshNumList[selectMeshId];
        }
        fontSmall.drawString(tSs.str(), pX, ofGetHeight() - 190);
        
        
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
            ::strftime(charDateTime, sizeof(charDateTime), "%Y-%m-%d %H:%M:%S", &tempTimeStruct);
            string strCharDateTime = charDateTime;
            tSs <<  "VirtualTime: " << strCharDateTime << "." << nowPlayTimeTemp%1000;
            fontSmall.drawString(tSs.str(), pX, ofGetHeight() - 150);
        }
        
        
        
        if (uiBtnPlayPause) {
    //        fontSmall.drawString("now playing...", 100, 700);
        }
        
        ofDisableDepthTest();
        if (viewerMode == 0) {
            ofSetColor(255, 0, 0, 192);
        } else {
            ofSetColor(128, 128, 128, 192);
        }
        //ofRect(0, 700, 200, 50);
        ofRectRounded(ofRectangle(10, 700, 180, 50), 10);
        
        ofSetColor(255,255,255,255);
        fontSmall.drawString("Detail", 40, 735);

        if (viewerMode == 1) {
            ofSetColor(255, 1, 0, 192);
        } else {
        //ofRect(0, 700, 200, 50);
            ofSetColor(128, 128, 128, 192);
        }
        ofRectRounded(ofRectangle(210, 700, 180, 50), 10);
        
        ofSetColor(255,255,255,255);
        fontSmall.drawString("File List", 240, 735);
        
        if (viewerMode == 0) {
            if (mouseX < 150 && mouseY < 500 && mouseY >= 150) {
                ofSetColor(255, 128, 128, 192);
            } else {
                ofSetColor(128, 128, 128, 192);
            }
            ofRectRounded(ofRectangle(35, ofGetHeight()/2+0, 80, 80), 40);
            
            if (mouseX >= (ofGetWidth()-150)&& mouseY < 500 && mouseY >= 150) {
                ofSetColor(255, 128, 128, 192);
            } else {
                ofSetColor(128, 128, 128, 192);
            }
            
            ofRectRounded(ofRectangle(ofGetWidth()-115, ofGetHeight()/2+0, 80, 80), 40);
            ofSetColor(255, 255, 255, 255);
            font.drawString("<", 60, ofGetHeight()/2+54);
            font.drawString(">", ofGetWidth()-87, ofGetHeight()/2+54);
        }
        
        // GUI‚Äû√á√≠√ã¬∞¬Æ√Å¬ß‚à´
        
        if (dispGui) {
            //guiMapEdit.draw();
            //gui.draw();
            guiPlayControl.draw();
            guiTabbedPages.draw();
        }
        
    }
    
    frameCount++;
    
    if (uiBtnPlayPause) {
        playCount++;
    }

    if (frameCount>1) {
        
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
// Main Draw Meshod end ----------------------------------------------------------------------------------


void ofApp::resetCam( ) {
    
    eCam.reset();
}

void ofApp::resetCamDetailView( ) {
    
    viewerMode = 0;
    
    eCam.reset();
    
    float modelSizeX = (modelSceneMax[selectMeshId].x - modelSceneMin[selectMeshId].x) *1000;
    float modelSizeY = (modelSceneMax[selectMeshId].y - modelSceneMin[selectMeshId].y) * 1000;
    float modelSizeZ = (modelSceneMax[selectMeshId].z - modelSceneMin[selectMeshId].z) * 1000;
    
    eCam.setPosition(0, -modelSizeZ*1.2, modelSizeZ*1.2);
    eCam.setTarget(ofVec3f(0, 0, 0));
    
}

void ofApp::resetCamListView( ) {
    
    viewerMode = 1;
    
    eCam.reset();
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
    
    /*
     ofSetColor(128,255,128, 64);
     for(int j=-1; j<=20; j++) {
     ofLine(0,1,100*j,2000,1,100*j);
     stringstream tSs;
     tSs.str("");
     if (j!= 0) {
     tSs << "" << j << "0cm";
     } else {
     tSs << "" << j << "cm";
     
     }
     
     }
     */
    
}


// ///////////////////////////////////////////////////////////////////////////////////////////////////
//   Control Functions
// ///////////////////////////////////////////////////////////////////////////////////////////////////


void ofApp::keyPressed(int key){
    
    cout << "keyPressed. key number: " << key << endl;
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
    if (key == ' ') {
        if (dispAllUiFlag == false) {
            dispAllUiFlag = true;
        } else {
            dispAllUiFlag = false;
        }
    }
    if (key == 57) {
        if (uiBtnPlayPause == false) {
            uiBtnPlayPause = true;
        } else {
            uiBtnPlayPause = false;
        }
        
    }
    
    if (key == 52) {     //
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
        
    }
    
    if (key == 53) {     //
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
    }
    
    if (key == 51) {
        if (uiBtnTimerControl) {
            uiBtnTimerControl = false;
        } else {
            uiBtnTimerControl = true;
        }
        
    }
    
    if (key == 48) {
        
        eCam.reset();
        eCam.setPosition(0, 0, 1000);
        viewerMode = 1;
        
        selectMeshId = 0;
        
        cout << "key number 0 Check!: " << key << endl;
    }
    
    if (int(key) == 49) {
        viewerMode = 0;
        selectMeshId = 0;
        eCam.reset();
        
        float modelSizeX = modelPosXList[selectMeshId]*1000 / 2;
        float modelSizeY = modelHeightList[selectMeshId]*1000 / 2;
        float modelSizeZ = modelPosZList[selectMeshId]*1000 / 2;
        
        eCam.setPosition(modelSizeX, -modelSizeZ-modelSizeZ*2, modelSizeZ*2);
        eCam.setTarget(ofVec3f(modelSizeX, -modelSizeZ, 0));
        
    }
    
    if (viewerMode == 0) {
        if (key == OF_KEY_LEFT) {
            detailViewNextModel(-1);
        }
        if (key == OF_KEY_RIGHT) {
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
    
    
    if (uiBtnTimerControl) {
    //if (y >= 500 && y < 700) {
        
        if (uiPlayMode == 2) {
            
            long seekbarCalcTime = (int)(((double)mouseX / (double)ofGetWidth()) * (scanUnixTimeAllItemMax - scanUnixTimeAllItemMin));
            
            seekbarAddTime = (scanUnixTimeAllItemMax - scanUnixTimeAllItemMin) - ((ofGetElapsedTimeMillis() - scanUnixTimeAllItemMin) - (seekbarCalcTime - scanUnixTimeAllItemMin));

        } else if (uiPlayMode == 1) {
            
            seekbarAddTime = (int)(((double)mouseX / (double)ofGetWidth()) * totalScanTimeRecordMaxTime);
            
        } else {
            
            playCount = (int)(((double)mouseX / (double)ofGetWidth() ) * totalMaxMeshNum );
            
        }
        
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

    //cout << "mousePressed. mouseX: " << mouseX << " mouseY: " << mouseY << endl;

    /*
    if (uiBtnTimerControl) {
    //if (y >= 500 && y < 700) {
        
        if (uiPlayMode == 2) {
            
            long seekbarCalcTime = (int)(((double)mouseX / (double)ofGetWidth()) * (scanUnixTimeAllItemMax - scanUnixTimeAllItemMin));
            
            seekbarAddTime = (scanUnixTimeAllItemMax - scanUnixTimeAllItemMin) - ((ofGetElapsedTimeMillis() - scanUnixTimeAllItemMin) - (seekbarCalcTime - scanUnixTimeAllItemMin));
            
            //uiBtnPlayPause = false;
            
        } else if (uiPlayMode == 1) {
            
            seekbarAddTime = (int)(((double)mouseX / (double)ofGetWidth()) * totalScanTimeRecordMaxTime);
            //uiBtnPlayPause = false;
            
        } else {
            
            playCount = (int)(((double)mouseX / (double)ofGetWidth() ) * totalMaxMeshNum );
            //uiBtnPlayPause = false;
            
        }
        
    }
     */
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
    ofPoint mousePos = ofPoint(x, y);
    
    if (uiBtnTimerControl){
        return;
    }
    
    if (x<100 && y<100) {
        if (!dispGui) {
            dispGui = true;
        } else {
            dispGui = false;
        }
        
    } else if (myGuiMainMenu.inside(mousePos)) {
        
        if (myGuiMainMenu.inside(mousePos)) {    // メニュー選択
        
            if (x >= 0 && x < 200) {
                selectMeshId = 0;
                resetCamDetailView();
            }

            if (x >= 200 && x < 400) {
                
                selectMeshId = 0;
                resetCamListView();
            }
            
        }
        
    } else if (myGuiDetailLeftButton.inside(mousePos)) {
        
        if (viewerMode == 0 ) {    // 左右のモデルセレクタ
        
            detailViewNextModel(-1);
            
        }
        
    } else if (myGuiDetailRightButton.inside(mousePos)) {
        
        if (viewerMode == 0 ) {    // 左右のモデルセレクタ
            
            detailViewNextModel(1);
            
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
    
    modeldataDatasetNum = 0;
    modeldataFiles = 0;
    
    ofDirectory ofDir;
    ofDir.listDir(meshDataDirPath);
    
    vector<ofFile> files = ofDir.getFiles();
    vector<ofFile>::iterator itr = files.begin();
    int tCount = 0;
    while( itr != files.end() )
    {
        if (itr->isDirectory()) {
            string s = itr->getFileName();
            cout << s << endl;
            dataDirNameList.push_back(s);
            
            if (!mapFileExists) {
                mapId[tCount] = s;
            }
            tCount++;
        }
        ++itr;
    }
    
    for(int i=0; i<MAX_MODEL_ARRAY; i++) {
        modelSceneMin[i].x = INT_MAX;
        modelSceneMin[i].y = INT_MAX;
        modelSceneMin[i].z = INT_MAX;
        modelSceneMax[i].x = 0;
        modelSceneMax[i].y = 0;
        modelSceneMax[i].z = 0;
    }
    

    
    //  --------------------------------------------------------------------
    
    // Assimp model loader
    int dirNameLoopCount = 0;
    for (auto dirName : dataDirNameList)
    {
        stringstream ss;
        ofSetWindowTitle(ss.str());
        
        {
            cout << "dirName: " << dirName << endl;
            
            stringstream dirPath;
            dirPath.str("");
            int meshFileNum;
            meshFileNum = 0;
            
            ofDirectory ofDir;
            //dirPath << "./" << dirName << "/";
            dirPath << meshDataDirPath << "/" << dirName << "/";
            ofDir.listDir(dirPath.str());
            //ofDir.listDir(meshDataDirPath);
            
            
            meshNameList[dirNameLoopCount] = dirName;
            
            bool staticModelFlag = false;
            
            vector<ofFile> files = ofDir.getFiles();
            vector<ofFile>::iterator itr = files.begin();  //
            while( itr != files.end() )  //
            {
                //cout << "*" << itr->getFileName() << endl;  //
                if (!itr->isDirectory()) {
                    string fName = itr->getFileName();
                    if (fName.substr(0,4) == "mesh" && fName.substr(fName.size()-3,3) == "obj") {
                        meshFileNum++;
                        
                        //cout << "***" << itr->getFileName() << endl;  //
                    }
                    
                    if (itr->getFileName().substr(0,9) == "Model.obj") {
                        meshFileNum++;
                        staticModelFlag = true;
                    }
                    
                    //dataDirNameList.push_back(itr->getFileName());
                }
                ++itr;                 //
            }
            
            maxMeshNumList[dirNameLoopCount] = meshFileNum;
            
            cout << "mesh file num: " << meshFileNum << endl;  // *√ä¬∫√Æ√Å√Ü√≥√Ç‚â†√™‚Äû√Ö√ü√à√±√¨√ä√©‚Ä¢√Ç√®√á√Å√ñ√ü
            
            if (staticModelFlag == false) {

                struct tm tempTmStruct;
                memset(&tempTmStruct,0x00,sizeof(struct tm));               // Initialize important!

                // getTimestamp
                ss.str("");
                ss << dirPath.str() << "scanTimeRecord.csv";

                ofFileObj.open(ss.str());
                int idx = 0;
                if (ofFileObj.exists()) {
                    ofBuffer buffer(ofFileObj);
                    
                    int targetFrame = (idx*skipLoadFrame);
                    int maxFrameOverTest = targetFrame;
                    cout << "maxMeshNumList[dirNameLoopCount]:" << maxMeshNumList[dirNameLoopCount] << endl;
                    while (!buffer.isLastLine() && (idx < maxLoadMeshNum) && maxFrameOverTest < maxMeshNumList[dirNameLoopCount]) {
                        
                        string line;
                        line = buffer.getNextLine();
                        
                        //cout << ""line;
                        if (line != "") {
                            auto itemList = ofSplitString(line, ",");
                            //cout << itemList[1] << endl;
                            scanTimeRecordList[dirNameLoopCount][idx][0] = stoi(itemList[0]);
                            scanTimeRecordList[dirNameLoopCount][idx][1] = stoi(itemList[1]);
                            cout << "scanTimeRecordList[dirNameLoopCount][idx][1]: " << scanTimeRecordList[dirNameLoopCount][idx][1] << endl;
                            
                            //itemlist[2] ... "2016/10/09 17:26:56.432"
                            string strDateTime = itemList[2].substr(0, 19);           // 2016/10/09 17:26:56
                            string strMilliSec = itemList[2].substr(20, 3);           // .432
                            
                            const char *cStrDateTime = strDateTime.c_str();
                            
                            ::strptime(cStrDateTime, "%Y/%m/%d %H:%M:%S", &tempTmStruct);
                            
                            scanTimeRecordTimeStructureList[dirNameLoopCount][idx] = mktime(&tempTmStruct);
                            scanUnixTimeLongIntList[dirNameLoopCount][idx] = ((long)mktime(&tempTmStruct) * 1000) + stoi(strMilliSec);
                            
                            // min/max keep -----------------
                            if (scanUnixTimeModelMinList[dirNameLoopCount] >= scanUnixTimeLongIntList[dirNameLoopCount][idx]) {
                                scanUnixTimeModelMinList[dirNameLoopCount] = scanUnixTimeLongIntList[dirNameLoopCount][idx];
                            }
                            
                            if (scanUnixTimeModelMaxList[dirNameLoopCount] <= scanUnixTimeLongIntList[dirNameLoopCount][idx]) {
                                scanUnixTimeModelMaxList[dirNameLoopCount] = scanUnixTimeLongIntList[dirNameLoopCount][idx];
                            }
                            
                            if (scanUnixTimeAllItemMin >= scanUnixTimeLongIntList[dirNameLoopCount][idx]) {
                                scanUnixTimeAllItemMin = scanUnixTimeLongIntList[dirNameLoopCount][idx];
                            }
                            
                            if (scanUnixTimeAllItemMax <= scanUnixTimeLongIntList[dirNameLoopCount][idx]) {
                                scanUnixTimeAllItemMax = scanUnixTimeLongIntList[dirNameLoopCount][idx];
                            }
                            // --------------------------
                            
                            if (itemList.size() >= 10) {
                                for(int x=0; x<7; x++) {
                                    scanGpsDataList[dirNameLoopCount][idx][x] = stod(itemList[3+x]); // latitude
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
                                modelMatrixList[dirNameLoopCount] = ofMatrix4x4(floatMatrix);
                                cout << endl;
                            }
                            
                            idx++;
                            
                            //cout << strDateTime << "  ." << strMilliSec << " --- " << scanUnixTimeLongIntList[dirNameLoopCount][idx] << endl;
                        }
                        
                        for(int p=0; p<(skipLoadFrame-1); p++) {
                            line = buffer.getNextLine();
                        }

                    }
                    
                    scanTimeRecordMaxTime[dirNameLoopCount] = scanTimeRecordList[dirNameLoopCount][idx-1][1];
                    cout << "scanTimeRecordMaxTime: " << scanTimeRecordMaxTime[dirNameLoopCount] << endl;
                    
                    ofFileObj.close();
                    
                }
                
                //maxMeshNumList[dirNameLoopCount] = idx - 2;
                if (idx > maxLoadedMeshNumInAllMesh) {
                    maxLoadedMeshNumInAllMesh = idx;
                }
                
                oneModelFileSizeList.clear();
                
                for(int i=0; (i*skipLoadFrame+2)<maxMeshNumList[dirNameLoopCount]; i++) {
                    
                    auto& model = modelList[dirNameLoopCount][i];

                    
                    // getTimestamp
                    ss.str("");
                    ss << dirPath.str() << "mesh_" << ((i*skipLoadFrame)+2+startPlayMeshAnimNum) << ".obj";
                    //ss << "F:/ArtDKT_kuwakubo_3dscan_20160123to25/artdkt_3dscan_20160124_kouhan/artdkt_structure3d/38/mesh_" << (i+2) << ".obj";
                    string objFilePath = ss.str();
                    
                    //cout << ss.str() << endl;

                    ofFileObj.open(objFilePath);       //
                    if (ofFileObj.exists()) {
                        
                        oneModelFileSizeList.push_back(ofFileObj.getSize()); // getFileSize
                        
                        ofFileObj.close();
                        
                        if (loadVertexColorObj) {
                            ofxObjLoader::load(objFilePath, modelList[dirNameLoopCount][i], false);
                        }
                        
                        // add
                        ss.str("");
                        //ss << dirPath.str() << "mesh_" << ((i*skipLoadFrame)+2+startPlayMeshAnimNum) << ".jpg";
                        ss << objFilePath.substr(0,objFilePath.size()-4) << ".jpg";
                        string objImageFilePath = ss.str();
                        if (loadPictureMode) {
                            modelImageList[dirNameLoopCount][i].loadImage(objImageFilePath);
                        }
                        auto vertices = modelList[dirNameLoopCount][i].getVertices();
                        
                        float maxPosY = 0;
                        for(auto vertice : vertices) {
                            if (maxPosY <= vertice.y) {
                                maxPosY = vertice.y;
                            }
                        }
                        modelHeightList[dirNameLoopCount] = maxPosY;
                        
                        float maxPosX = 0;
                        for(auto vertice : vertices) {
                            if (maxPosX <= vertice.x) {
                                maxPosX = vertice.x;
                            }
                        }
                        modelPosXList[dirNameLoopCount] = maxPosX;

                        float maxPosZ = 0;
                        for(auto vertice : vertices) {
                            if (maxPosZ <= vertice.z) {
                                maxPosZ = vertice.z;
                            }
                        }
                        modelPosZList[dirNameLoopCount] = maxPosX;

                        cout << "vertice.y: " << maxPosY<< endl;
                        
                        if (dualColorSystem) {
                            //Assimp ver.
                            
                            asModelObj[dirNameLoopCount][i].loadModel(objFilePath );
                            
                            ofPoint tMin = asModelObj[dirNameLoopCount][i].getSceneMin();
                            ofPoint tMax = asModelObj[dirNameLoopCount][i].getSceneMax();
                            cout << "tMin: " << tMin << " tMax: " << tMax << endl;
                            
                            if (tMin.x == 0 && tMin.y == 0 && tMin.z == 0 && tMax.x == 0 && tMax.y == 0 && tMax.z == 0) {
                                cout << "======---------- Hit! ----------------==============================================" << endl;
                            } else  {
                            
                                if (tMin.x < modelSceneMin[dirNameLoopCount].x) {
                                    modelSceneMin[dirNameLoopCount].x = tMin.x;
                                }
                                if (tMin.y < modelSceneMin[dirNameLoopCount].y) {
                                    modelSceneMin[dirNameLoopCount].y = tMin.y;
                                }
                                if (tMin.z < modelSceneMin[dirNameLoopCount].z) {
                                    modelSceneMin[dirNameLoopCount].z = tMin.z;
                                }
                                if (tMax.x > modelSceneMax[dirNameLoopCount].x) {
                                    modelSceneMax[dirNameLoopCount].x = tMax.x;
                                }
                                if (tMax.y > modelSceneMax[dirNameLoopCount].y) {
                                    modelSceneMax[dirNameLoopCount].y = tMax.y;
                                }
                                if (tMax.z > modelSceneMax[dirNameLoopCount].z) {
                                    modelSceneMax[dirNameLoopCount].z = tMax.z;
                                }
                            }
   
                        }
                        
                    } else {
                        cout << objFilePath << " file not found." << endl;
                    }
                    
                    cout << "modelSceneMin[" << dirNameLoopCount << "]: " << modelSceneMin[dirNameLoopCount] << endl;
                    cout << "modelSceneMax[" << dirNameLoopCount << "]: " << modelSceneMax[dirNameLoopCount] << endl;
                    
                    /*

                     Assimp ver.
                     
                     //asModelObj[dirNameLoopCount][i].loadModel(ss.str(), true );   true:ÊúÄÈÅ©Âåñ„Åô„Çã„Å®ÂÆüË°åÊôÇ„Å´ËêΩ„Å°„Çã! oF0.9.3 OSX10.11.5 LLVM0.7.1

                    auto& model = asModelObj[dirNameLoopCount][i];
                     
                    model.loadModel(ss.str());
                    
                    //auto& material = model.getMaterialForMesh(0);
                    //material.begin();
                    //model.enableMaterials();
                    //model.enableTextures();

                    //model.draw(OF_MESH_FILL);
                    //material.end();
                    model.disableMaterials();
                    model.disableTextures();
                     */
                    
                    cout << "file load: " << ss.str() << endl;
                }
                modelFileSizeList.push_back(oneModelFileSizeList);
                
            } else {        // static model
                
                maxMeshNumList[dirNameLoopCount] = 1;
                scanTimeRecordList[dirNameLoopCount][0][0] = -1;
                scanTimeRecordMaxTime[dirNameLoopCount] = 0;
                
                ss.str("");
                ss << dirPath.str() << "/Model.obj";
                
                ofFileObj.open(ss.str());
                
                if (ofFileObj.exists()) {
                    
                    oneModelFileSizeList.push_back(ofFileObj.getSize()); // getFileSize
                    
                    ofFileObj.close();
                    
                    ofxObjLoader::load(ss.str(), modelList[dirNameLoopCount][0], false);
                }
                
                /*
                 Assimp ver
                asModelObj[dirNameLoopCount][0].loadModel(ss.str() , true);
                
                asModelObj[dirNameLoopCount][0].disableMaterials();
                asModelObj[dirNameLoopCount][0].disableTextures();
                 */
                
                cout << "Model load: " << ss.str() << endl;
                
            }
            
            //gui->addWidgetRight(new ofxUIDropDownList("PARTICLE_IMAGE", items,150));
        }
        
        modeldataFiles += maxMeshNumList[dirNameLoopCount];

        /*
         */
        dirNameLoopCount++;
    }
    modelDataNum = dirNameLoopCount;
    //modeldataDatasetNum = dirNameLoopCount;
    
    loadFileSizeAll = 0;
    for(auto oneModelList : modelFileSizeList) {
        for(auto fileSize : oneModelFileSizeList) {
            loadFileSizeAll += fileSize;
        }
    }
    
    totalMaxMeshNum = 0;
    for (auto meshNum : maxMeshNumList) {
        if (meshNum > totalMaxMeshNum) {
            totalMaxMeshNum = meshNum;
        }
    }

    totalScanTimeRecordMaxTime = 0;
    for (auto time : scanTimeRecordMaxTime) {
        if (time > totalScanTimeRecordMaxTime) {
            totalScanTimeRecordMaxTime = time;
        }
    }
    
    cout<<"modelDataNum:"<<modelDataNum<<endl;
    
    modeldataLoadingEndTime = ofGetElapsedTimeMillis();

}

