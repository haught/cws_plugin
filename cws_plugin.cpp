// cws_plugin.cpp : �������̨Ӧ�ó������ڵ㡣
//

#if defined(_WIN32) || defined(WIN32)
#include "stdafx.h"
#endif

#if defined(__linux__)
#include <cstring>
#endif

#include<stdio.h>
#include<stdlib.h>
#include <map>

using namespace std;

#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <vector>



int startsWith(string s, string sub) {
	return s.find(sub) == 0?1 : 0;
}

int endsWith(string s, string sub) {
	return s.rfind(sub) == (s.length() - sub.length())?1 : 0;
}


std::vector<std::string> split(const char *s, const char *delim)
{
	std::vector<std::string> result;
	if (s && strlen(s))
	{
		int len = strlen(s);
		char *src = new char[len + 1];
		strcpy(src, s);
		src[len] = '\0';
		char *tokenptr = strtok(src, delim);
		while (tokenptr != NULL)
		{
			std::string tk = tokenptr;
			result.emplace_back(tk);
			tokenptr = strtok(NULL, delim);
		}
		delete[] src;
	}
	return result;
}

void write_1_gcode(char *fileName,map<string, string> &valueMap) {
	ofstream ofile;               //��������ļ�
	int layerNum = static_cast<int>(strtol(valueMap.at("totalLayer").c_str(), NULL, 0));
	int bottomLayerNum = static_cast<int>(strtol(valueMap.at("bottomLayCount").c_str(), NULL, 0));
	int normalExposureTime = static_cast<float>(strtof(valueMap.at("normalExposureTime").c_str(), NULL))*1000;
	int bottomLayExposureTime = static_cast<float>(strtof(valueMap.at("bottomLayExposureTime").c_str(), NULL)) * 1000;
	int bottomLightOffTime = static_cast<float>(strtof(valueMap.at("bottomLightOffTime").c_str(), NULL)) * 1000;
	int lightOffTime = static_cast<float>(strtof(valueMap.at("lightOffTime").c_str(), NULL)) * 1000;
	int resolutionX = static_cast<int>(strtol(valueMap.at("resolutionX").c_str(), NULL, 0));
	int resolutionY = static_cast<int>(strtol(valueMap.at("resolutionY").c_str(), NULL, 0));
	float widthX = static_cast<float>(strtof(valueMap.at("machineX").c_str(), NULL));
	float widthY = static_cast<float>(strtof(valueMap.at("machineY").c_str(), NULL));
	float layerHeight = static_cast<float>(strtof(valueMap.at("layerHeight").c_str(), NULL));
	ofile.open(fileName);     //��Ϊ����ļ���
	ofile << ";(****Build and Slicing Parameters****)" << endl;
	ofile << ";(Pix per mm X = " << resolutionX/widthX << " px/mm)" << endl;
	ofile << ";(Pix per mm Y = " << resolutionY/widthY << " px/mm)" << endl;
	ofile << ";(X Resolution = " << resolutionX << ")" << endl;
	ofile << ";(Y Resolution = " << resolutionY << ")" << endl;
	ofile << ";(Layer Thickness = " << layerHeight << " mm)" << endl;
	ofile << ";(Layer Time = 7000 ms)" << endl;
	ofile << ";(Bottom Layers Time = " << bottomLayExposureTime << " ms)" << endl;
	ofile << ";(Number of Bottom Layers = " << valueMap.at("bottomLayCount") << ")" << endl;
	ofile << ";(Blanking Layer Time = " << int(static_cast<float>(strtof(valueMap.at("normalExposureTime").c_str(), NULL)) * 1000) << " ms)" << endl;
	ofile << ";(Build Direction = Bottom_Up)" << endl;
	ofile << ";(Lift Distance = " << valueMap.at("normalLayerLiftHeight") << " mm)" << endl; 
	ofile << ";(Slide/Tilt Value = 0)" << endl;
	ofile << ";(Anti Aliasing = True)" << endl;
	ofile << ";(Use Mainlift GCode Tab = False)" << endl;
	ofile << ";(Anti Aliasing Value = 2)" << endl;
	ofile << ";(Z Lift Feed Rate = 60.00000 mm/s)" << endl;
	ofile << ";(Z Lift Retract Rate = 150.00000 mm/s)" << endl;
	ofile << ";(Flip X = " << ((valueMap.at("mirror") == "1")?"True":"False") << ")" << endl;
	ofile << ";(Flip Y = False)" << endl;
	ofile << ";Number of Slices = " << valueMap.at("totalLayer") <<  endl;
	ofile << ";(****Machine Configuration ******)" << endl;
	ofile << ";(Platform X Size = " << valueMap.at("machineX") << "mm)" << endl;
	ofile << ";(Platform Y Size = " << valueMap.at("machineY") << "mm)" << endl;
	ofile << ";(Platform Z Size = " << valueMap.at("machineZ") << "mm)" << endl;
	ofile << ";(Max X Feedrate = 100mm/s)" << endl;
	ofile << ";(Max Y Feedrate = 100mm/s)" << endl;
	ofile << ";(Max Z Feedrate = 100mm/s)" << endl;
	ofile << ";(Machine Type = UV_DLP)" << endl;
	ofile << "; ********** Header Start ********" << endl;
	ofile << "; Here you can set any G or M - Code which should be executed BEFORE the build process" << endl;
	ofile << "G21; Set units to be mm" << endl;
	ofile << "G91; Relative Positioning" << endl;
	ofile << "M17; Enable motors" << endl;
	ofile << "G28 Z0" << endl;
	ofile << "<Slice> Blank" << endl;
	ofile << "; ********** Header End **********" << endl;

	float normalLayerLiftHeight = static_cast<float>(strtof(valueMap.at("normalLayerLiftHeight").c_str(), NULL)) ;

	float dropHeight = normalLayerLiftHeight - layerHeight;
	for (int i = 0; i < layerNum; i++) {
		ofile << "M106 S255" << endl;

		ofile << ";********** Pre - Slice Start ********" << endl;
		ofile << ";Set up any GCode here to be executed before a lift" << endl;
		ofile << ";********** Pre - Slice End **********" << endl;
		ofile << ";<Slice> " << i << endl;
		ofile << ";<Delay> " << ((i < bottomLayerNum)? bottomLayExposureTime:normalExposureTime) << endl;
		ofile << ";<Slice> Blank" << endl;
		ofile << "M106 S0" << endl;
		ofile << "; ********** Lift Sequence ********" << endl;
		ofile << "G1 Z" << valueMap.at("normalLayerLiftHeight") << " F" << ((i < bottomLayerNum)?valueMap.at("bottomLayerLiftSpeed"):valueMap.at("normalLayerLiftSpeed")) << endl; 
		ofile << "G1 Z-" << dropHeight << " F" << valueMap.at("normalDropSpeed") << endl;
		ofile << ";<Delay> " << ((i < bottomLayerNum)?bottomLightOffTime : lightOffTime) << endl;
	}

	ofile << "; ********** Footer Start ********" << endl;
	ofile << "; Here you can set any G or M - Code which should be executed after the last Layer is Printed" << endl;
	ofile << "M18; Disable Motors" << endl;
	ofile << "M106 SO" << endl;
	ofile << "G1 Z40" << endl;
	ofile << ";<Completed>" << endl;
	ofile << "; ********** Footer End ********" << endl;

	ofile.close();
}

void writeManifest_xml(char *dirPath,char *fileName,map<string, string> &valueMap) {
	ofstream ofile;               //��������ļ�
	ofile.open(fileName);     //��Ϊ����ļ���
	
	int layerNum = static_cast<int>(strtol(valueMap.at("totalLayer").c_str(), NULL, 0));
	ofile << "<?xml version = \"1.0\" encoding = \"utf-8\"?>" << endl;
	ofile << "<manifest FileVersion = \"1\">" << endl;

	ofile << "<GCode>" << endl;
	ofile << "  <name>1.gcode</name>" << endl;
	ofile << "</GCode>" << endl;

	ofile << "<ScenePreview>" << endl;
	ofile << "  <Default>preview.png</Default>" << endl;
	ofile << "</ScenePreview>" << endl;


	ofile << "<Slices>" << endl;

	char *format, *fileFormat;
	int totalLayer = layerNum;
	if (totalLayer < 10000) {
		format = (char*)"%s/1%04d.png";
		fileFormat = (char*)"1%04d.png";
	}
	else if (totalLayer >= 10000 && totalLayer < 100000) {
		format = (char*)"%s/1%05d.png";
		fileFormat = (char*)"1%05d.png";
	}
	else if (totalLayer >= 100000 && totalLayer < 1000000) {
		format = (char*)"%s/1%06d.png";
		fileFormat = (char*)"1%06d.png";
	}
	else if (totalLayer >= 1000000 && totalLayer < 10000000) {
		format = (char*)"%s/1%07d.png";
		fileFormat = (char*)"1%07d.png";
	}
	else if (totalLayer >= 10000000 && totalLayer < 100000000) {			//impossible
		format = (char*)"%s/1%08d.png";
		fileFormat = (char*)"1%08d.png";
	}
	else {
		format = (char*)"%s/1%020d.png";
		fileFormat = (char*)"1%020d.png";
	}
	char oldName[4096], newName[4096];
	for (int i = 0; i < totalLayer; i++) {
		snprintf(oldName, sizeof(oldName), "%s/%d.png", dirPath, i + 1);
		snprintf(newName, sizeof(newName), format, dirPath, i + 1);
		rename(oldName, newName);
	}

	for (int i = 0; i < layerNum; i++) {
		ofile << "  <Slice>" << endl;
		snprintf(newName, sizeof(newName), fileFormat, i + 1);
		ofile << "		<name>" << string(newName) << "</name>" << endl;
		ofile << "  </Slice>" << endl;
	}
	ofile << "</Slices>" << endl;

	ofile << "<VectorSlices/>" << endl;

	ofile << "<SliceProfile>" << endl;
	ofile << "	<name>default.slicing</name>" << endl;
	ofile << "</SliceProfile>" << endl;


	ofile << "</manifest>" << endl;

	ofile.close();
}

void writeDefault_slicing(char *fileName,map<string, string> &valueMap ) {
	ofstream ofile;               //��������ļ�
	ofile.open(fileName);     //��Ϊ����ļ���
	int resolutionX = static_cast<int>(strtol(valueMap.at("resolutionX").c_str(), NULL, 0));
	int resolutionY = static_cast<int>(strtol(valueMap.at("resolutionY").c_str(), NULL, 0));
	float widthX = static_cast<float>(strtof(valueMap.at("machineX").c_str(), NULL));
	float widthY = static_cast<float>(strtof(valueMap.at("machineY").c_str(), NULL));
	ofile << "<?xml version = \"1.0\" encoding = \"utf-8\"?>" << endl;
	ofile << "<SliceBuildConfig FileVersion = \"2\">" << endl;
		
	ofile << "<DotsPermmX>" << resolutionX/widthX  <<  "</DotsPermmX>" << endl;
	ofile << "<DotsPermmY>" << resolutionY/widthY << "</DotsPermmY>" << endl;
	ofile << "<XResolution>" << valueMap.at("resolutionX") << "</XResolution>" << endl;
	ofile << "<YResolution>" << valueMap.at("resolutionY") << "</YResolution>" << endl;
	ofile << "<BlankTime>" << int(static_cast<float>(strtof(valueMap.at("normalExposureTime").c_str(), NULL)) * 1000) << "</BlankTime>" << endl;
	ofile << "<PlatformTemp>75</PlatformTemp>" << endl;
	ofile << "<ExportSVG>0</ExportSVG>" << endl;
	ofile << "<Export>True</Export>" << endl;
	ofile << "<ExportPNG>True</ExportPNG>" << endl;
	ofile << "<XOffset>0</XOffset>" << endl;
	ofile << "<YOffset>0</YOffset>" << endl;
	ofile << "<Direction>Bottom_Up</Direction>" << endl;
	ofile << "<LiftDistance>" << valueMap.at("normalLayerLiftHeight") << "</LiftDistance>" << endl;
	ofile << "<SlideTiltValue>0</SlideTiltValue>" << endl;
	ofile << "<AntiAliasing>True</AntiAliasing>" << endl;
	ofile << "<UseMainLiftGCode>False</UseMainLiftGCode>" << endl;
	ofile << "<AntiAliasingValue>2</AntiAliasingValue>" << endl;
	ofile << "<LiftFeedRate>" << static_cast<int>(strtol(valueMap.at("normalLayerLiftSpeed").c_str(), NULL, 0))  << "</LiftFeedRate>" << endl;
	ofile << "<BottomLiftFeedRate>" << static_cast<int>(strtol(valueMap.at("bottomLayerLiftSpeed").c_str(), NULL, 0))  << "</BottomLiftFeedRate>" << endl;
	ofile << "<LiftRetractRate>" << static_cast<int>(strtol(valueMap.at("normalDropSpeed").c_str(), NULL, 0))  << "</LiftRetractRate>" << endl;
	ofile << "<ExportOption>ZIP</ExportOption>" << endl;
	ofile << "<RenderOutlines>False</RenderOutlines>" << endl;
	ofile << "<OutlineWidth_Inset>0</OutlineWidth_Inset>" << endl;
	ofile << "<OutlineWidth_Outset>0</OutlineWidth_Outset>" << endl;
	ofile << "<FlipX>" << (valueMap.at("mirror") == "1"?"True": "False") << "</FlipX>" << endl;
	ofile << "<FlipY>False</FlipY>" << endl;

	ofile << "<Notes>" << endl;
	ofile << "</Notes>" << endl;
	ofile << "<GCodeHeader>; ********** Header Start ********" << endl;
	ofile << "; Here you can set any G or M - Code which should be executed BEFORE the build process" << endl;
	ofile << "G21; Set units to be mm" << endl;
	ofile << "G91; Relative Positioning" << endl;
	ofile << "M17; Enable motors" << endl;
	ofile << "G28 Z0" << endl;
	ofile << "&lt; Slice&gt; Blank" << endl;
	ofile << "; ********** Header End **********" << endl;
	ofile << "M106 S255" << endl;

	ofile << "</GCodeHeader>" << endl;
	ofile << "<GCodeFooter>; ********** Footer Start ********" << endl;
	ofile << "; Here you can set any G or M - Code which should be executed after the last Layer is Printed" << endl;
	ofile << "M18; Disable Motors" << endl;
	ofile << "M106 SO" << endl;
	ofile << "G1 Z40" << endl;
	ofile << "; &lt; Completed&gt;" << endl;
	ofile << "; ********** Footer End ********" << endl;
	ofile << "</GCodeFooter>" << endl;
	ofile << "<GCodePreslice>; ********** Pre - Slice Start ********" << endl;
	ofile << "; Set up any GCode here to be executed before a lift" << endl;
	ofile << "; ********** Pre - Slice End **********" << endl;
	ofile << "</GCodePreslice>" << endl;
	ofile << "<GCodeLift>M106 S0" << endl;
	ofile << "; ********** Lift Sequence ********" << endl;
	ofile << "G1{ $SlideTiltVal != 0?X$SlideTiltVal : } Z($ZLiftDist * $ZDir) F$ZLiftRate" << endl;
	ofile << "G1 {" << endl;
	ofile << "$SlideTiltVal != 0?X($SlideTiltVal * -1) :" << endl;
	ofile << "} Z(($LayerThickness - $ZLiftDist) * $ZDir) F$ZRetractRate" << endl;
	ofile << "; &lt; Delay&gt; %d$BlankTime" << endl;
	ofile << "M106 S255" << endl;
	ofile << "; ********** Lift Sequence **********" << endl;

	ofile << "</GCodeLift>" << endl;
	ofile << "<SelectedInk>Default</SelectedInk>" << endl;
	ofile << "<InkConfig>" << endl;
	ofile << "<Name>Default</Name>" << endl;
	ofile << "<SliceHeight>" << valueMap.at("layerHeight") << "</SliceHeight>" << endl; 
	ofile << "<LayerTime>" << int(static_cast<float>(strtof(valueMap.at("normalExposureTime").c_str(), NULL))*1000)  << "</LayerTime>" << endl;
	ofile << "<FirstLayerTime>" << int(static_cast<float>(strtof(valueMap.at("bottomLayExposureTime").c_str(), NULL)) * 1000) << "</FirstLayerTime>" << endl;
	ofile << "<NumberofBottomLayers>" << valueMap.at("bottomLayCount") << "</NumberofBottomLayers>" << endl; 
	ofile << "<ResinPriceL>0</ResinPriceL>" << endl;
	ofile << "</InkConfig>" << endl;
	ofile << "<MinTestExposure>500</MinTestExposure>" << endl;
	ofile << "<TestExposureStep>200</TestExposureStep>" << endl;
	ofile << "<ExportPreview>Top</ExportPreview>" << endl;
	ofile << "<UserParameters/>" << endl;
	ofile << "<EnableWipe>False</EnableWipe>" << endl;
	ofile << "<WipeBFLift>False</WipeBFLift>" << endl;
	ofile << "<EnableLed>True</EnableLed>" << endl;
	ofile << "<LedRate>255</LedRate>" << endl;
	ofile << "<FlowTime>0</FlowTime>" << endl;
	ofile << "<DrawTime>0</DrawTime>" << endl;
	ofile << "<WipeRate>50</WipeRate>" << endl;
	ofile << "<WipeStart>3</WipeStart>" << endl;
	ofile << "<WipeEnd>6</WipeEnd>" << endl;
	ofile << "</SliceBuildConfig>" << endl;
	ofile.close();
}

void readGcode(char *fileName, map<string, string> &valueMap)
{
	ifstream infile;
	infile.open(fileName);   //���ļ����������ļ��������� 
	assert(infile.is_open());   //��ʧ��,�����������Ϣ,����ֹ�������� 
	
	string s;
	std::vector<std::string> ss;
	bool headInit = false;
	while (getline(infile, s))
	{
		//cout << s << endl;
		if (s.at(0) == ';') {
			if (startsWith(s, ";START_GCODE_BEGIN")) {
				headInit = true;
				break;
			}else{
				ss = split(s.data() + 1, ":");
				if (ss.size() == 2) {
					valueMap.insert(pair<string, string>(ss.at(0), ss.at(1)));
				}
			}
		}
	}
	infile.close();             //�ر��ļ������� 
}

int main(int argc, char **argv) {
	int i;
	map<string, string> valueMap;
	char fileName[1024];
	for (int i = 0; i < argc; i++) {
		cout << "arg" <<  i << ":" << argv[i] << endl;
	}
	if (argc <3) {
		cout << "error:Invalid parameter,format:" << argv[0] << "input output" << endl;	//Error to UI
		exit(0);
	}
	sprintf(fileName, "%s/run.gcode",argv[1]);
	readGcode(fileName, valueMap);


	sprintf(fileName, "%s/default.slicing", argv[1]);
	writeDefault_slicing(fileName, valueMap);

	sprintf(fileName, "%s/manifest.xml", argv[1]);
	writeManifest_xml(argv[1],fileName, valueMap);

	sprintf(fileName, "%s/1.gcode", argv[1]);
	write_1_gcode(fileName, valueMap);



	cout << "progress:0" << endl;				//progross to UI

	cout << "info:" << endl;				//progross to UI

}

