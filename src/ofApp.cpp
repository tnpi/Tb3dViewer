#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    appInitStartTime = ofGetElapsedTimeMillis();
    
    defaultMeshDataDirPath = "/Users/artdkt/Desktop/3dscan_data_for0630/artdkt_structure3d";

    // setting vars --------------------------------------------

    dualColorSystem = true;           // 同じモデルデータを２つのライブラリで同時に読み込み、切り替えながら表示します。２倍動作に時間がかかり、メモリ消費も２倍です。
    colorMode = 1;

    viewerMode = 1;
    //uiMeshDrawType = 1;
    selectMeshId = 0;
    mapFileExists = false;
    mapDataColumns = 16;
    
    totalLoadedModelNum = 0;
    maxLoadedMeshNumInAllMesh = 0;
    
    maxLoadMeshNum = 4;

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
    ofxGuiSetFont("hira.otf", 13, true, true);
    ofxGuiSetTextPadding(4);
    ofxGuiSetDefaultWidth(300);
    
    // OfxGUI setup -----------------------------------------------------------------
    
    ofColor initColor = ofColor(0, 127, 255, 255);
    ofColor minColor = ofColor(0,0,0,0);
    ofColor maxColor = ofColor(255,255,255,255);
 
    ofVec2f initPos = ofVec2f(ofGetWidth(), ofGetHeight());
    ofVec2f minPos = ofVec2f(-ofGetWidth() * 3, -ofGetHeight() * 4);
    ofVec2f maxPos = ofVec2f(ofGetWidth() * 2, ofGetHeight() * 3);
   
    uiBtnPlayPause.setSize(100, 100);
    
    gui.setup("settings");
    //gui.setFillColor(<#const ofColor &color#>)
    
    gui.setDefaultBackgroundColor(ofColor(255,0,0));
    
    gui.setPosition(500, 50);
    gui.setDefaultHeight(30);

    // Add GUI parts -------------------------------------------------------------------------------

    gui.add(uiThumbnailIconDistance.setup("thumbnailIconDistance", 3000, 0, 5000));
    gui.add(uiIconNumX.setup("iconNumX", 4, 1, 8));
    gui.add(uiMeshDrawType.setup("meshDrawType", 0, 0, 2));
    gui.add(uiColorMode.setup("colorMode", 0, 0, 1));
    gui.add(uiPlayMode.setup("playMode", 0, 0, 2));
    gui.add(uiFramerate.setup("framerate", 60, 5, 60));
    gui.add(uiBtnPlayPause.setup("Play / Stop", true, 40, 40));
    gui.add(uiBtnLight.setup("Light on/off", true, 40, 40));
    gui.add(uiBtnGrid.setup("Grid", false, 40, 40));
    gui.add(uiBtnDebugInfo.setup("DebugInfo", false, 40, 40));
    gui.add(uiBtnTurnMesh.setup("TurnMesh", true, 40, 40));
    gui.add(uiBtnLoopPlay.setup("LoopPlay", true, 40, 40));
    gui.add(uiBtnOrtho.setup("Ortho", false, 40, 40));
    gui.add(uiGpsMapMode.setup("GPSMapMode", 0, 0, 2));
    gui.add(uiTestSlider.setup("TestSlider", 0 ,  -10000, 10000));
    gui.add(uiBtnReset.setup("Reset", 40, 40));
    gui.add(uiBtnSelectReset.setup("quit", 40, 40));
    
    guiMapEdit.setup("MapEdit");
    guiMapEdit.setPosition(800, 100);
    guiMapEdit.setDefaultWidth(500);
    guiMapEdit.setDefaultHeight(30);
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
    
    uiBtnSelectReset.addListener(this, &ofApp::turnMeshSwitch);
    
    guiPlayItem.setup("playItem");
    
    guiPlayItem.add(uiBtnPlaySelectA.setup("A", 50, 40));
    guiPlayItem.add(uiBtnPlaySelectB.setup("B", 50, 40));
    guiPlayItem.add(uiBtnPlaySelectC.setup("C", 50, 40));
    guiPlayItem.add(uiBtnPlaySelectBack.setup("Reset", 50, 40));
    
    
    appInitEndTime = ofGetElapsedTimeMillis();

    
    playStartDateTime = ofGetElapsedTimeMillis();

    uiPlayMode = playMode;
 
    ::time(&unixTimeOnOfStarted);

}


void ofApp::turnMeshSwitch(){
    
    OF_EXIT_APP(0);
    
}

//--------------------------------------------------------------
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
    
    if (mouseY >= (ofGetHeight() - 200)) {
        eCam.disableMouseInput();
    } else {
        eCam.enableMouseInput();
    }
    
    if (prevFramePlayState == false && uiBtnPlayPause) {
        playStartPrevPos = nowPlayTime;
        playStartDateTime = ofGetElapsedTimeMillis();
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofSetFrameRate(uiFramerate);

    displayTotalVertices = 0;
    
    if (frameCount == 1) {  // app start
        dataLoad();
    }
    
    // --------------------------
    ofBackground(240, 240, 240);        // gray bg
    
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

    
    if (viewerMode == 0) {
        eCam.disableMouseInput();
    } else {
        eCam.enableMouseInput();
    }
    
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
        
        ofSetColor(255,255,255);
        
        if (viewerMode == 0 && selectMeshId != i) {
            continue;
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
        
        ofSetLineWidth(1);
        
        if (viewerMode == 0) {
            
            fboCam.begin();
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
            
            
            
            fboCam.end();
            
            
            float modelSizeX = modelPosXList[selectMeshId]*1000;
            float modelSizeY = modelHeightList[selectMeshId]*1000;
            float modelSizeZ = modelPosZList[selectMeshId]*1000;

            
            asModelObj[i][playFrameSelector].draw(OF_MESH_FILL);
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
            ofSetColor(128,224,255, 64);
            ofLine(0,0,0,0,0,0);
            //ofRect(0, 0, 10000, 10000);
            
            ofSetLineWidth(3);
            ofSetColor(128,224,255, 128);
            for(int j=-1; j<=40; j++) {
                ofLine(-100,100*j,1,modelSizeX*2,100*j,1);
                stringstream tSs;
                tSs.str("");
                if (j!= 0) {
                    tSs << "" << j << "0cm";
                } else {
                    tSs << "" << j << "cm";
                    
                }
                font.drawString(tSs.str(), -300, 100*j+19);
                
            }

            ofSetColor(128,224,255, 128);
            for(int j=-1; j<=40; j++) {
                ofLine(100*j,-100,1,100*j,modelSizeZ*2,1);
                stringstream tSs;
                tSs.str("");
                if (j!= 0) {
                    tSs << "" << j << "0cm";
                } else {
                    tSs << "" << j << "cm";
                    
                }
                
                ofPushMatrix();
                ofTranslate(100*j+19, -140, 0);
                ofRotate(270, 0,0,1);
                font.drawString(tSs.str(), 0, 0);
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

            // 6/29
            //ofScale(1, -1);

            
            if (dualColorSystem == true && uiColorMode == 1) {

                //ofTranslate(asModelObj[i][playFrameSelector].getSceneCenter().x, asModelObj[i][playFrameSelector].getSceneCenter().y, -asModelObj[i][playFrameSelector].getSceneCenter().z);
                double centerX = modelSceneMin[i].x + (modelSceneMax[i].x - modelSceneMin[i].x) / 2;
                double centerY = modelSceneMin[i].y + (modelSceneMax[i].x - modelSceneMin[i].x) / 2;
                double centerZ = modelSceneMin[i].z + (modelSceneMax[i].x - modelSceneMin[i].x) / 2;
                ofTranslate(centerX, centerY, -centerZ);
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
                    ofSetLineWidth(1);
                    modelList[i][playFrameSelector].drawWireframe();
                } else if (uiMeshDrawType == 2) {
                    glPointSize(5);
                    modelList[i][playFrameSelector].drawVertices();
                } else {
                    modelList[i][playFrameSelector].draw();
                }
            }
            
            /*

            if (uiMeshDrawType == 1) {
                //asModelObj[i][counter].draw(OF_MESH_WIREFRAME);
                //            asModelObj[i][counter].drawWireframe();
                ofSetLineWidth(1);
                modelList[i][playFrameSelector].drawWireframe();
            } else if (uiMeshDrawType == 2) {
                //asModelObj[i][counter].draw(OF_MESH_POINTS);
                //asModelObj[i][counter].draw(OF_MESH_POINTS);
                //stroke(10);
                glPointSize(5);
                ofBlendMode(OF_BLENDMODE_ALPHA);

                modelList[i][playFrameSelector].drawVertices();
            } else {
                
                modelList[i][playFrameSelector].draw();
                
                //asModelObj[i][counter].drawFaces();
                //asModelObj[i][counter].draw(OF_MESH_FILL);
            }
             */
            
            glPopMatrix();
            
            glPushMatrix();
            glRotatef(90, 1, 0, 0);
            ofTranslate(0,0,1000);
            ofScale(10,10,10);
            modelImageList[i][playFrameSelector].draw(0,0);
            glPopMatrix();

            
        }
        else if (viewerMode <= 1) {
            
            // draw 3axis
            ofSetLineWidth(10);
            ofSetColor(255,64,64);
            ofLine(0,0,0,10000,0,0);
            font.drawString("X", 10200, 0);
            ofSetColor(64,192,64);
            ofLine(0,0,0,0,10000,0);
            font.drawString("Y", 0, 10200);
            ofSetColor(64,64,255);
            ofLine(0,0,0,0,0,10000);
            
            // draw base grid
            ofSetColor(128,224,255, 64);
            ofLine(0,0,0,0,0,0);
            ofRect(0, 0, 10000, 10000);
            
            ofSetLineWidth(5);
            ofSetColor(128,224,255, 128);
            for(int j=0; j<=10; j++) {
                ofLine(0,1000*j,1,10000,1000*j,1);
                stringstream tSs;
                tSs.str("");
                tSs << "" << j << "m";
                fontLarge.drawString(tSs.str(), -300, 1000*j+50);
                
            }
            
            // d
            for(int j=0; j<=10; j++) {
                ofLine(1000*j,0,1,1000*j,10000,1);
                
                stringstream tSs;
                tSs.str("");
                tSs << "" << j << "m";
                fontLarge.drawString(tSs.str(), 1000*j-100, -100);
            }
            
            displayTotalVertices += modelList[i][playFrameSelector].getNumVertices();
            //displayTotalFaces += modelList[i][counter].getNum;
            
            if (uiGpsMapMode == 0) {
                glPushMatrix();
                
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
                    fontLarge.drawString(dataDirNameList[i],0,0);        // display model name
                    ofEnableLighting();

                    //font.drawString(to_string(asModelObj[i][counter].getNumMeshes()),600,500);
                    glPopMatrix();
                }
                
                
                if (uiBtnTurnMesh) {
                    glRotatef(-90, 1, 0, 0);
                    
                }
                
                /*
                // tempolary ( for combine)
                if (i == 1) {
                    glRotatef(180, 0, 1, 0);
                    ofTranslate(0,0,-530);
                }
                */

                if (mapNum[i][7] >= 1) {
                    glRotatef(180, 0, 1, 0);
                    ofTranslate(0,0,-530);
                }
                if (mapNum[i][8] >= 1) {
                    glRotatef(180, 1, 0, 0);
                    //ofTranslate(0,0,-530);
                }

                ofTranslate(0,-1*modelHeightList[i]*1000,0);
                ofTranslate(0,-0,modelPosZList[i]*1000);        // hosei
                
                glTranslatef(mapNum[i][0], mapNum[i][1], mapNum[i][2]);
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
                    
                    if (dualColorSystem == true && uiColorMode == 1) {
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

            } else {        // GPS Map mode
                
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
        
    }
    
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
    
    if (dispAllUiFlag) {
    
        tSs.str("");
        tSs << "FPS: " << fixed << setprecision(1) << ofGetFrameRate() << "fps" << resetiosflags(ios_base::floatfield);
        fontSmall.drawString(tSs.str(), 40, 80);
        
        tSs.str("");
        tSs << "Vertices: " << displayTotalVertices << "pts";
        fontSmall.drawString(tSs.str(), 40, 120);
        
        tSs.str("");
        tSs << "AppInitTime: " << (appInitEndTime - appInitStartTime) << "ms";
        fontSmall.drawString(tSs.str(), 40, 160);

        tSs.str("");
        tSs << "modelLoadingTime: " << (modeldataLoadingEndTime - modeldataLoadingStartTime) << "ms";
        fontSmall.drawString(tSs.str(), 40, 200);
        
        tSs.str("");
        tSs << "modelNum: " << modelDataNum;
        fontSmall.drawString(tSs.str(), 40, 240);
        
        tSs.str("");
        tSs << "files: " << modeldataFiles;
        fontSmall.drawString(tSs.str(), 40, 280);

        tSs.str("");
        tSs << "totalLoadFileSize: " << (loadFileSizeAll/1000/1000) << "MB";
        fontSmall.drawString(tSs.str(), 40, 320);

        tSs.str("");
        tSs << "PlayStartFrame: " << startPlayMeshAnimNum;
        fontSmall.drawString(tSs.str(), 40, 360);
        
        tSs.str("");
        tSs << "maxLoadMeshNum: " << maxLoadMeshNum;
        fontSmall.drawString(tSs.str(), 40, 400);
        
        tSs.str("");
        tSs << "maxLoadedModelNum: " << maxLoadedMeshNumInAllMesh;
        fontSmall.drawString(tSs.str(), 40, 440);

        if (viewerMode == 0) {
            tSs.str("");
            tSs << "meshName: " << meshNameList[selectMeshId];
            fontSmall.drawString(tSs.str(), 40, 480);
        }

        ofSetColor(255,255,255,255);
        
        if (uiBtnDebugInfo) {
            stringstream tSs;
            tSs.str("");
            tSs << "mouseX: " << mouseX << " mouseY: " << mouseY << "eCam.x: " << eCam.getX() << " eCam.y: " << eCam.getY() << " eCam.z" << eCam.getZ();
            
            fontSmall.drawString(tSs.str(), 50, 700);

            
            ofVec3f worldPos = eCam.screenToWorld(ofVec3f(mouseX, mouseY, 0));
            stringstream tSs2;
            tSs2.str("");
            tSs2 << "worldX: " << worldPos.x << " worldY: " << worldPos.y << " worldZ: " << worldPos.z;
            fontSmall.drawString(tSs2.str(), 50, 750);

            stringstream tSs3;
            
            tSs3 << "viewerMode: " << viewerMode << endl;
            
            fontSmall.drawString(tSs3.str(), 50, 800);
        }

        if (mouseX >= gui.getPosition().x
            && mouseX < (gui.getPosition().x + gui.getWidth())
            && mouseY >= gui.getPosition().y
            && mouseY < (gui.getPosition().y + gui.getHeight()) ) {
            eCam.disableMouseInput();
        } else {
            eCam.enableMouseInput();
        }
        
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
        fontSmall.drawString(tSs.str(), 40, ofGetHeight() - 230);

        tSs.str("");
        if (viewerMode == 1) {
            tSs << "playCount: " << playCount << "   /  endMeshNum: " << totalMaxMeshNum;
        } else if (viewerMode == 0) {
            tSs << "playCount: " << playCount << "   /  endMeshNum: " << maxMeshNumList[selectMeshId];
        }
        fontSmall.drawString(tSs.str(), 40, ofGetHeight() - 190);
        
        
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
            fontSmall.drawString(tSs.str(), 40, ofGetHeight() - 150);
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
            guiMapEdit.draw();
            gui.draw();
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
                    playCount %= totalMaxMeshNum;
                } else if (viewerMode == 0) {
                    playCount %= maxMeshNumList[selectMeshId];
                }
            }
            
        }
        
    }

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    cout << "key number: " << key << endl;
    
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
    if (key == 49) {
        if (uiBtnPlayPause == false) {
            uiBtnPlayPause = true;
        } else {
            uiBtnPlayPause = false;
        }
        
        
    }
    
    if (key == 48) {
        
        eCam.reset();
        eCam.setPosition(0, 0, 1000);
        viewerMode = 1;
        
        selectMeshId = 0;
        
        cout << "key number 0 Check!: " << key << endl;
    }

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
    
    if (y >= 500 && y < 700) {
        
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

    cout << "mouseX: " << mouseX << " mouseY: " << mouseY << endl;
    
    if (y >= 500 && y < 700) {
        
        if (uiPlayMode == 2) {
            
            long seekbarCalcTime = (int)(((double)mouseX / (double)ofGetWidth()) * (scanUnixTimeAllItemMax - scanUnixTimeAllItemMin));
            
            seekbarAddTime = (scanUnixTimeAllItemMax - scanUnixTimeAllItemMin) - ((ofGetElapsedTimeMillis() - scanUnixTimeAllItemMin) - (seekbarCalcTime - scanUnixTimeAllItemMin));
            
            //uiBtnPlayPause = false;
            
        } else if (uiPlayMode == 1) {
            
            seekbarAddTime = (int)(((double)mouseX / (double)ofGetWidth()) * totalScanTimeRecordMaxTime);
            uiBtnPlayPause = false;
            
        } else {
            
            playCount = (int)(((double)mouseX / (double)ofGetWidth() ) * totalMaxMeshNum );
            uiBtnPlayPause = false;
            
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
    
    
    cout << "program exit." << endl;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
    if (x<100 && y<100) {
        if (!dispGui) {
            dispGui = true;
        } else {
            dispGui = false;
        }
    }
    
    if (y >= 700 && y<900) {
        
        if (x >= 0 && x < 200) {
            selectMeshId = 0;
            resetCamDetailView();
        }

        if (x >= 200 && x < 400) {
            
            selectMeshId = 0;
            resetCamListView();
        }
    }
    
    if (viewerMode == 0 ) {
        if (x < 150 && y < 500 && y >= 150) {
            
            selectMeshId = ((selectMeshId - 1) + modelDataNum) % modelDataNum;
            resetCamDetailView();
            
        }
        if (x >= (ofGetWidth()-150) && y < 500 && y >= 150) {
            
            selectMeshId = (selectMeshId + 1) % modelDataNum;
            resetCamDetailView();
            
        }
    }
    
    

    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}

void ofApp::resetCam( ) {
    
    eCam.reset();
}

void ofApp::resetCamDetailView( ) {
    
    viewerMode = 0;
    
    eCam.reset();
    
    float modelSizeX = modelPosXList[selectMeshId]*1000 / 2;
    float modelSizeY = modelHeightList[selectMeshId]*1000 / 2;
    float modelSizeZ = modelPosZList[selectMeshId]*1000 / 2;
    
    eCam.setPosition(modelSizeX, -modelSizeZ-modelSizeZ*2, modelSizeZ*2);
    eCam.setTarget(ofVec3f(modelSizeX, -modelSizeZ, 0));
    
}

void ofApp::resetCamListView( ) {

    viewerMode = 1;
    
    eCam.reset();
    eCam.setPosition(3000, -3000, 9000);
    eCam.setTarget(ofVec3f(3000, -3000, 0));
}


void ofApp::setPos( ) {
    
    auto posX = position->x;
    auto posY = position->y;
    
}

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
        
        // ----------------------------------------------------------------------------------
        // Read file line by line
        // ------------------------------------------------------------------------------
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


// Read mesh files ----------------------------------------------------------------
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
                    
                    while (!buffer.isLastLine() && idx < maxLoadMeshNum) {
                        string line = buffer.getNextLine();
                        //cout << ""line;
                        if (line != "") {
                            auto itemList = ofSplitString(line, ",");
                            //cout << itemList[1] << endl;
                            scanTimeRecordList[dirNameLoopCount][idx][0] = stoi(itemList[0]);
                            scanTimeRecordList[dirNameLoopCount][idx][1] = stoi(itemList[1]);
                            
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
                            
                            
                            //cout << strDateTime << "  ." << strMilliSec << " --- " << scanUnixTimeLongIntList[dirNameLoopCount][idx] << endl;
                            idx++;
                        }
                        
                    }
                    scanTimeRecordMaxTime[dirNameLoopCount] = scanTimeRecordList[dirNameLoopCount][idx-1][1];
                    cout << "scanTimeRecordMaxTime: " << scanTimeRecordMaxTime[dirNameLoopCount] << endl;
                    
                    ofFileObj.close();
                    
                }
                
                maxMeshNumList[dirNameLoopCount] = idx - 2;
                if (idx > maxLoadedMeshNumInAllMesh) {
                    maxLoadedMeshNumInAllMesh = idx;
                }
                
                oneModelFileSizeList.clear();
                
                for(int i=0; i<maxMeshNumList[dirNameLoopCount]; i++) {
                    
                    auto& model = modelList[dirNameLoopCount][i];
                    
                    ofFileObj.open(ss.str());
                    if (ofFileObj.exists()) {

                        ss.str("");
                        ss << dirPath.str() << "mesh_" << (i+2+startPlayMeshAnimNum) << ".obj";
                        //ss << "F:/ArtDKT_kuwakubo_3dscan_20160123to25/artdkt_3dscan_20160124_kouhan/artdkt_structure3d/38/mesh_" << (i+2) << ".obj";
                        
                        cout << ss.str() << endl;
                        
                        string objFilePath = ss.str();

                        
                        oneModelFileSizeList.push_back(ofFileObj.getSize()); // getFileSize
                        
                        ofFileObj.close();
                        ofxObjLoader::load(objFilePath, modelList[dirNameLoopCount][i], false);
                        
                        
                        // add
                        ss.str("");
                        ss << dirPath.str() << "mesh_" << (i+2+startPlayMeshAnimNum) << ".jpg";
                        modelImageList[dirNameLoopCount][i].loadImage(ss.str());
                        
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

                        
                        cout << "vertice.y: " << maxPosY;
                        
                        
                        if (dualColorSystem) {
                            //Assimp ver.
                            
                            asModelObj[dirNameLoopCount][i].loadModel(objFilePath );
                            
                            ofPoint tMin = asModelObj[dirNameLoopCount][i].getSceneMin();
                            ofPoint tMax = asModelObj[dirNameLoopCount][i].getSceneMax();
                            if (tMin.x < modelSceneMin[dirNameLoopCount].x || (modelSceneMin[dirNameLoopCount].x == 0)) {
                                modelSceneMin[dirNameLoopCount].x = tMin.x;
                            }
                            if (tMin.y < modelSceneMin[dirNameLoopCount].y || (modelSceneMin[dirNameLoopCount].y == 0)) {
                                modelSceneMin[dirNameLoopCount].y = tMin.y;
                            }
                            if (tMin.z < modelSceneMin[dirNameLoopCount].z || (modelSceneMin[dirNameLoopCount].z == 0)) {
                                modelSceneMin[dirNameLoopCount].z = tMin.z;
                            }
                            if (tMax.x > modelSceneMax[dirNameLoopCount].x || (modelSceneMax[dirNameLoopCount].x == 0)) {
                                modelSceneMax[dirNameLoopCount].x = tMax.x;
                            }
                            if (tMax.y < modelSceneMax[dirNameLoopCount].y || (modelSceneMax[dirNameLoopCount].y == 0)) {
                                modelSceneMax[dirNameLoopCount].y = tMax.y;
                            }
                            if (tMax.z < modelSceneMax[dirNameLoopCount].z || (modelSceneMax[dirNameLoopCount].z == 0)) {
                                modelSceneMax[dirNameLoopCount].z = tMax.z;
                            }
                            
                                
   
                        }
                        
                    } else {
                        cout << dirPath.str() << "mesh_" << (i+2+startPlayMeshAnimNum) << ".obj file not found" << endl;
                    }
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

