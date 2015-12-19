//
//  baseScene.cpp
//  d4nSFPCRunner
//
//  Created by zach on 11/14/15.
//
//

#include "baseScene.h"


void baseScene::loadCode( string fileName ){
    
    ofBuffer buffer = ofBufferFromFile(fileName);
    code = "";
    
    for (auto line : buffer.getLines()){
        code += line;
        code += "\n";
    }

    // Remove trailing newlines and spaces
    code.erase(std::find_if(code.rbegin(),
                            code.rend(),
                            std::not1(std::ptr_fun<int, int>(std::isspace))).base(),
               code.end());
    dataPath = ofFilePath::removeTrailingSlash(ofFilePath::getEnclosingDirectory(fileName));
    if (ofFile(dataPath+"/paramsRecording.xml").exists()) {
        loadMidi(recData, dataPath+"/paramsRecording.xml");
    }
    bHasEndSet = false;
}

void baseScene::enableMidi() {
    ofParameter<bool>   boolParam;
    ofParameter<int>    intParam;
    ofParameter<float>  floatParam;
    
    paramTypes.clear();
    boolParams.clear();
    intParams.clear();
    floatParams.clear();
    
    for (auto param : parameters) {
        if (param->type() == boolParam.type()) {
            ofParameter<bool> newParam;
            ofParameter<bool> &oldParam = param->cast<bool>();
            newParam.setName(oldParam.getName());
            newParam.setMin(oldParam.getMin());
            newParam.setMax(oldParam.getMax());
            newParam.set(oldParam.get());
            boolParams.push_back(newParam);
            midiParameters.add(boolParams.back());
            paramTypes.push_back('b');
        } else if (param->type() == intParam.type()) {
            ofParameter<int> newParam;
            ofParameter<int> &oldParam = param->cast<int>();
            newParam.setName(oldParam.getName());
            newParam.setMin(oldParam.getMin());
            newParam.setMax(oldParam.getMax());
            newParam.set(oldParam.get());
            intParams.push_back(newParam);
            midiParameters.add(intParams.back());
            paramTypes.push_back('i');
        } else if (param->type() == floatParam.type()) {
            ofParameter<float> newParam;
            ofParameter<float> &oldParam = param->cast<float>();
            newParam.setName(oldParam.getName());
            newParam.setMin(oldParam.getMin());
            newParam.setMax(oldParam.getMax());
            newParam.set(oldParam.get());
            floatParams.push_back(newParam);
            midiParameters.add(floatParams.back());
            paramTypes.push_back('f');
        }
    }
}

void baseScene::updateMidiParams() {
    int nToCheck = min((int)parameters.size(), 8);
    for (int i = 0; i < nToCheck; ++i) {
        char paramType = paramTypes[i];
        
        if (paramType == 'b') {
            ofParameter<bool> &midiParam = midiParameters.getBool(i);
            ofParameter<bool> &param = parameters.getBool(i);
            param.set(midiParam);
        } else if (paramType == 'i') {
            ofParameter<int> &midiParam = midiParameters.getInt(i);
            ofParameter<int> &param = parameters.getInt(i);
            param.set(midiParam);
        } else if (paramType == 'f') {
            ofParameter<float> &midiParam = midiParameters.getFloat(i);
            ofParameter<float> &param = parameters.getFloat(i);
            
            if (isnan(midiParam.get()))
                continue;
            
            float diff = midiParam - param;
            if (fabs(diff) > 0.0001)
                param.set(param + diff * baseScene::smoothingSpeed);
            else
                param.set(midiParam);
        }
    }
}

bool baseScene::isSceneDone(){
    return (getElapsedTimef()>sceneDuration);
}
void baseScene::setSceneEnd(){
    sceneDuration = getElapsedTimef();
    bHasEndSet = true;
}
bool baseScene::hasRecData(){
    return recData.size() != 0;
}
void baseScene::setRecData(const vector<ofxMidiRecordingEvent>& data){
    recData.clear();
    recData = data;
    cout << "setRecData: " << recData.size() << "  " << dataPath << endl;
    //if (!dataPath.empty()) {
        saveMidi(recData, dataPath+"/paramsRecording.xml");
    //}
}
const vector<ofxMidiRecordingEvent>& baseScene::getRecData(){
    return recData;
}
void baseScene::setAuthor(string newAuthor) {
    author = newAuthor;
}

void baseScene::setOriginalArtist(string newOriginalArtist) {
    originalArtist = newOriginalArtist;
}

float baseScene::getElapsedTimef(){
    return (ofGetElapsedTimeMicros() - startTime)/1000000.;
}
uint64_t baseScene::getFrameNum(){
    return ofGetFrameNum() - startFrame;
}

void baseScene::resetTiming(){
    startTime = ofGetElapsedTimeMicros();
    startFrame = ofGetFrameNum();
}
