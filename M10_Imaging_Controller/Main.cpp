// Basler Camera Control Code with M10 App integration
// 2016, Andrew Petersen & Stebner Research Group

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include "TCPServer.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <Windows.h>

#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif
#include <pylon/gige/PylonGigEIncludes.h>
#include <pylon/gige/ActionTriggerConfiguration.h>

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "5200"
#define DEFAULT_DIR "C:/Users/Administrator/BaslerImages/"
#define DEFAULT_STEM "Basler-acA645-"
#define DEFAULT_EXTEN ".tiff"
#define IMAGE_WIDTH 659
#define IMAGE_HEIGHT 494
#define MAXBUFFER 100
#define IMAGESIZE 325546 //image size in bytes
#define TERMINATOR "-"
#define DEFAULT_TIMEOUT 5000


#define SERIALNUM_R_NEW "21494796"
#define SERIALNUM_L_NEW "21719635" 

using namespace Pylon;
using namespace std;

string getParamFromFile(string filepath, string pName);

int main(int argc, char* argv[])
{
	String_t fname, fileDir = DEFAULT_DIR, fileStem = DEFAULT_STEM, fileExten = DEFAULT_EXTEN;
	int imageCount_L = 0;
	int imageCount_R = 0;
	char printfBuff1[100];
	char printfBuff2[100];

	int imageCount_L2 = 0;
	int imageCount_R2 = 0;
	char printfBuff12[100];
	char printfBuff22[100];

	int extendedNum = 0;


	char inputBuffer[DEFAULT_BUFLEN];
	int bytesRead;


	int timeout = DEFAULT_TIMEOUT;
	string port = DEFAULT_PORT;

	cout << endl << "# of cmd line arguments: " << argc << endl;
	if (argc > 5 ){
		stringstream ss;
		ss << argv[1];

		ss >> timeout;
		timeout = 2000 / (double)timeout;
		port = argv[2];
		fileDir = argv[3];
		fileStem = argv[4];
		fileExten = argv[5];

		cout << "Command Line Parameters:" << endl << endl
			<< "Command Timeout (ms): " << timeout << endl
			<< "Port: " << port << endl
			<< "File Output Directory: " << fileDir << endl
			<< "File Stem: " << fileStem << endl
			<< "File Extention: " << fileExten << endl
			<< endl;
	}
	else {
		//cout << getParamFromFile("M10_Imaging_Controller_config.txt", "cameraL1_serialNum") << endl;
		cout << "Command line inputs not detected, closing program" << endl;
		return 0;
	}



	//open socket on port 5200, blocks until connection recieved
	TCP_Server server(port);

	//Closes down program if an error occurred
	if (!server.IsConnected()) {
		cout << "An error occurred establishing a connection, closing down program" << endl;
		system("PAUSE");
		return -1;
	}

	//sets the data read timeout
	////server.SetTimeout(timeout);

	//Camera Initialization variables
	//Camera device pointers
	IPylonDevice* dev_cameraL = NULL;
	IPylonDevice* dev_cameraR = NULL;
	IPylonDevice* dev_cameraL2 = NULL;
	IPylonDevice* dev_cameraR2 = NULL;

	//Camera pointers (initalized from devices)
	CInstantCamera* cameraL = NULL;
	CInstantCamera* cameraR = NULL;
	CInstantCamera* cameraL2 = NULL;
	CInstantCamera* cameraR2 = NULL;

	//Image grabg result pointers
	CGrabResultPtr grL;
	CGrabResultPtr grR;
	CGrabResultPtr grL2;
	CGrabResultPtr grR2;


	////////////////////////////Open DIC Properties///////////////////////////////////////

	ifstream ifs;
	ifs.open("");

	if (ifs.fail())
		return 1;

	string l1, l2;
	//ifs.getline()





	////////////////////////////CAMERA INITIALIZATION/////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////

	//Initalizes Pylon
	Pylon::PylonAutoInitTerm autoInitTerm;

	//Attempts to initalize the left camera1
	while (true) {
		try {
			string s_num = getParamFromFile("M10_Imaging_Controller_config.txt", "cameraL1_serialNum");
			if (s_num == "")
				break;

			if (getParamFromFile("M10_Imaging_Controller_config.txt", "cameraL1_type") == "GigE") {
				dev_cameraL = (IPylonDevice*)CTlFactory::GetInstance().CreateDevice(CDeviceInfo().SetDeviceClass(BaslerGigEDeviceClass).SetSerialNumber(s_num.c_str()));
			}
			else if (getParamFromFile("M10_Imaging_Controller_config.txt", "cameraL1_type") == "USB") {
				dev_cameraL = (IPylonDevice*)CTlFactory::GetInstance().CreateDevice(CDeviceInfo().SetDeviceClass(BaslerUsbDeviceClass).SetSerialNumber(s_num.c_str()));
			}
			else {
				break;
			}

			cameraL = new CInstantCamera(dev_cameraL, Cleanup_Delete);
			cout << "Camera LEFT1 initialization successful" << endl;
			break;
		}
		catch (GenICam::GenericException &e) {
			cout << "An error occurred initializing the LEFT1 camera:" << endl;
			cout << e.GetDescription() << endl;
			if (e.GetDescription()[23] != 'X' && e.GetDescription()[23] != 'M' && e.GetDescription()[23] != 'L')
				break;
		}
	}

	//Attempts to initialize the right camera1
	while (true) {
		try {
			string s_num = getParamFromFile("M10_Imaging_Controller_config.txt", "cameraR1_serialNum");
			if (s_num == "")
				break;

			if (getParamFromFile("M10_Imaging_Controller_config.txt", "cameraR1_type") == "GigE") {
				dev_cameraR = (IPylonDevice*)CTlFactory::GetInstance().CreateDevice(CDeviceInfo().SetDeviceClass(BaslerGigEDeviceClass).SetSerialNumber(s_num.c_str()));
			}
			else if (getParamFromFile("M10_Imaging_Controller_config.txt", "cameraR1_type") == "USB") {
				dev_cameraR = (IPylonDevice*)CTlFactory::GetInstance().CreateDevice(CDeviceInfo().SetDeviceClass(BaslerUsbDeviceClass).SetSerialNumber(s_num.c_str()));
			}
			else {
				break;
			}

			cameraR = new CInstantCamera(dev_cameraR, Cleanup_Delete);
			cout << "Camera RIGHT1 initialization successful" << endl;
			break;
		}
		catch (GenICam::GenericException &e) {
			cout << "An error occurred initializing the RIGHT camera:" << endl;
			cout << e.GetDescription() << endl;
			if (e.GetDescription()[23] != 'X' && e.GetDescription()[23] != 'M' && e.GetDescription()[23] != 'L')
				break;
		}
	}

	//Attempts to initalize the left camera2
	while (true) {
		try {
			string s_num = getParamFromFile("M10_Imaging_Controller_config.txt", "cameraL2_serialNum");
			if (s_num == "")
				break;

			if (getParamFromFile("M10_Imaging_Controller_config.txt", "cameraL2_type") == "GigE") {
				dev_cameraL2 = (IPylonDevice*)CTlFactory::GetInstance().CreateDevice(CDeviceInfo().SetDeviceClass(BaslerGigEDeviceClass).SetSerialNumber(s_num.c_str()));
			}
			else if (getParamFromFile("M10_Imaging_Controller_config.txt", "cameraL2_type") == "USB") {
				dev_cameraL2 = (IPylonDevice*)CTlFactory::GetInstance().CreateDevice(CDeviceInfo().SetDeviceClass(BaslerUsbDeviceClass).SetSerialNumber(s_num.c_str()));
			}
			else {
				break;
			}

			cameraL2 = new CInstantCamera(dev_cameraL2, Cleanup_Delete);
			cout << "Camera LEFT2 initialization successful" << endl;
			break;
		}
		catch (GenICam::GenericException &e) {
			cout << "An error occurred initializing the LEFT2 camera:" << endl;
			cout << e.GetDescription() << endl;
			if (e.GetDescription()[23] != 'X' && e.GetDescription()[23] != 'M' && e.GetDescription()[23] != 'L')
				break;
		}
	}

	//Attempts to initialize the right camera2
	while (true) {
		try {
			string s_num = getParamFromFile("M10_Imaging_Controller_config.txt", "cameraR2_serialNum");
			if (s_num == "")
				break;

			if (getParamFromFile("M10_Imaging_Controller_config.txt", "cameraR2_type") == "GigE") {
				dev_cameraR2 = (IPylonDevice*)CTlFactory::GetInstance().CreateDevice(CDeviceInfo().SetDeviceClass(BaslerGigEDeviceClass).SetSerialNumber(s_num.c_str()));
			}
			else if (getParamFromFile("M10_Imaging_Controller_config.txt", "cameraR2_type") == "USB") {
				dev_cameraR2 = (IPylonDevice*)CTlFactory::GetInstance().CreateDevice(CDeviceInfo().SetDeviceClass(BaslerUsbDeviceClass).SetSerialNumber(s_num.c_str()));
			}
			else {
				break;
			}

			cameraR2 = new CInstantCamera(dev_cameraR2, Cleanup_Delete);
			cout << "Camera RIGHT2 initialization successful" << endl;
			break;
		}
		catch (GenICam::GenericException &e) {
			cout << "An error occurred initializing the RIGHT2 camera:" << endl;
			cout << e.GetDescription() << endl;
			if (e.GetDescription()[23] != 'X' && e.GetDescription()[23] != 'M' && e.GetDescription()[23] != 'L')
				break;
		}
	}


	//Registers LEFT1 camera configuration
	if (cameraL != NULL) {
		while (true) {
			try {
				cameraL->RegisterConfiguration(new CSoftwareTriggerConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);
				cout << "Camera LEFT1 Configuration Registered" << endl;
				break;
			}
			catch (GenICam::GenericException &e) {
				cout << "An error occurred registering LEFT1 camera configuration." << endl;
				cout << e.GetDescription() << endl;
			}
		}
	}

	//Registers RIGHT1 camera configuration
	if (cameraR != NULL) {
		while (true) {
			try {
				cameraR->RegisterConfiguration(new CSoftwareTriggerConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);
				cout << "Camera RIGHT1 Configuration Registered" << endl;
				break;
			}
			catch (GenICam::GenericException &e) {
				cout << "An error occurred registering RIGHT1 camera configuration." << endl;
				cout << e.GetDescription() << endl;
			}
		}
	}

	//Registers LEFT2 camera configuration
	if (cameraL2 != NULL) {
		while (true) {
			try {
				cameraL2->RegisterConfiguration(new CSoftwareTriggerConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);
				cout << "Camera LEFT2 Configuration Registered" << endl;
				break;
			}
			catch (GenICam::GenericException &e) {
				cout << "An error occurred registering LEFT2 camera configuration." << endl;
				cout << e.GetDescription() << endl;
			}
		}
	}

	//Registers RIGHT2 camera configuration
	if (cameraR2 != NULL) {
		while (true) {
			try {
				cameraR2->RegisterConfiguration(new CSoftwareTriggerConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);
				cout << "Camera RIGHT2 Configuration Registered" << endl;
				break;
			}
			catch (GenICam::GenericException &e) {
				cout << "An error occurred registering RIGHT2 camera configuration." << endl;
				cout << e.GetDescription() << endl;
			}
		}
	}

	//Starts camera grabbing
	try {
		if (cameraL != NULL) {
			cameraL->StartGrabbing(GrabStrategy_LatestImageOnly, GrabLoop_ProvidedByUser);
		}
	}
	catch (GenICam::GenericException &e) {
		cout << "Camera LEFT1 grabbing failed: " << e.GetDescription() << endl;
	}

	try {
		if (cameraR != NULL) {
			cameraR->StartGrabbing(GrabStrategy_LatestImageOnly, GrabLoop_ProvidedByUser);
		}
	}
	catch (GenICam::GenericException &e) {
		cout << "Camera RIGHT1 grabbing failed: " << e.GetDescription() << endl;
	}

	try {
		if (cameraL2 != NULL) {
			cameraL2->StartGrabbing(GrabStrategy_LatestImageOnly, GrabLoop_ProvidedByUser);
		}
	}
	catch (GenICam::GenericException &e) {
		cout << "Camera LEFT2 grabbing failed: " << e.GetDescription() << endl;
	}

	try {
		if (cameraR2 != NULL) {
			cameraR2->StartGrabbing(GrabStrategy_LatestImageOnly, GrabLoop_ProvidedByUser);
		}
	}
	catch (GenICam::GenericException &e) {
		cout << "Camera RIGHT2 grabbing failed: " << e.GetDescription() << endl;
	}

	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////

	//Tells m10 software it's ready to take picture
	server.writeData("r", 1);
	for (;;) {
		if (!server.IsConnected())
			break;
		server.readData(inputBuffer, DEFAULT_BUFLEN);
		cout << inputBuffer[0] << endl;
		if (inputBuffer[0] == 'r')
			break;
	}
	cout << "Ready to take pictures, waiting for capture commands..." << endl;

	while (server.IsConnected()) {
		//cout << server.IsConnected() << endl;
		bytesRead = server.readData(inputBuffer, DEFAULT_BUFLEN);
		printf("Bytes Recieved = %i\r\n", bytesRead);
		//cout << (char)inputBuffer[0] << endl;
		if (bytesRead > 0 && inputBuffer[0] == 'a') {
			cout << "Image capture command received." << endl;
			////////////////////IMAGE CAPTURE CODE///////////////////////////////
			/////////////////////////////////////////////////////////////////////

			//LEFT Image capture code
			try {
				if (cameraL != NULL && cameraL->WaitForFrameTriggerReady(100, TimeoutHandling_ThrowException)) {
					cameraL->ExecuteSoftwareTrigger();
					cameraL->RetrieveResult(5000, grL, TimeoutHandling_ThrowException);
					cout << "LEFT Image Captured" << endl;
				}
			}
			catch (GenICam::GenericException &e) {
				cout << "An error occurred capturing the LEFT image:" << endl;
				cout << e.GetDescription() << endl;
			}

			try {
				if (cameraL != NULL) {
					sprintf_s(printfBuff1, "%03.4i", imageCount_L);
					sprintf_s(printfBuff2, "%03.4i", extendedNum);

					fname = fileDir + fileStem + "_L1_" + printfBuff2 + "_" + printfBuff1 + fileExten;
					CImagePersistence::Save(ImageFileFormat_Tiff, fname, grL);
					imageCount_L++;
					cout << "LEFT Image Saved #" << imageCount_L << endl;
					imageCount_L++;
				}
			}
			catch (GenICam::GenericException &e) {
				cout << "An error occurred saving the LEFT image:" << endl;
				cout << e.GetDescription() << endl;
			}

			//RIGHT image capture code
			try {
				if (cameraR != NULL && cameraR->WaitForFrameTriggerReady(100, TimeoutHandling_ThrowException)) {
					cameraR->ExecuteSoftwareTrigger();
					cameraR->RetrieveResult(5000, grR, TimeoutHandling_ThrowException);
					cout << "RIGHT Image Captured" << endl;
				}
			}
			catch (GenICam::GenericException &e) {
				cout << "An error occurred capturing the RIGHT image:" << endl;
				cout << e.GetDescription() << endl;
			}

			try {
				if (cameraR != NULL) {
					sprintf_s(printfBuff1, "%03.4i", imageCount_R);
					sprintf_s(printfBuff2, "%03.4i", extendedNum);

					fname = fileDir + fileStem + "_R1_" + printfBuff2 + "_" + printfBuff1 + fileExten;
					CImagePersistence::Save(ImageFileFormat_Tiff, fname, grR);
					imageCount_R++;
					cout << "RIGHT Image Saved #" << imageCount_R << endl;
				}
			}
			catch (GenICam::GenericException &e) {
				cout << "An error occurred saving the RIGHT image:" << endl;
				cout << e.GetDescription() << endl;
			}


			//SECONDARY SET

			//LEFT2 Image capture code
			try {
				if (cameraL2 != NULL && cameraL2->WaitForFrameTriggerReady(100, TimeoutHandling_ThrowException)) {
					cameraL2->ExecuteSoftwareTrigger();
					cameraL2->RetrieveResult(5000, grL2, TimeoutHandling_ThrowException);
					cout << "LEFT2 Image Captured" << endl;
				}
			}
			catch (GenICam::GenericException &e) {
				cout << "An error occurred capturing the LEFT2 image:" << endl;
				cout << e.GetDescription() << endl;
			}

			try {
				if (cameraL2 != NULL) {
					sprintf_s(printfBuff12, "%03.4i", imageCount_L2);
					sprintf_s(printfBuff22, "%03.4i", extendedNum);

					fname = fileDir + fileStem + "_L2_" + printfBuff22 + "_" + printfBuff12 + fileExten;
					CImagePersistence::Save(ImageFileFormat_Tiff, fname, grL2);
					imageCount_L2++;
					cout << "LEFT2 Image Saved #" << imageCount_L2 << endl;
					imageCount_L2++;
				}
			}
			catch (GenICam::GenericException &e) {
				cout << "An error occurred saving the LEFT2 image:" << endl;
				cout << e.GetDescription() << endl;
			}

			//RIGHT image capture code
			try {
				if (cameraR2 != NULL && cameraR2->WaitForFrameTriggerReady(100, TimeoutHandling_ThrowException)) {
					cameraR2->ExecuteSoftwareTrigger();
					cameraR2->RetrieveResult(5000, grR2, TimeoutHandling_ThrowException);
					cout << "RIGHT2 Image Captured" << endl;
				}
			}
			catch (GenICam::GenericException &e) {
				cout << "An error occurred capturing the RIGHT2 image:" << endl;
				cout << e.GetDescription() << endl;
			}

			try {
				if (cameraR2 != NULL) {
					sprintf_s(printfBuff12, "%03.4i", imageCount_R2);
					sprintf_s(printfBuff22, "%03.4i", extendedNum);

					fname = fileDir + fileStem + "_R2_" + printfBuff22 + "_" + printfBuff12 + fileExten;
					CImagePersistence::Save(ImageFileFormat_Tiff, fname, grR2);
					imageCount_R2++;
					cout << "RIGHT2 Image Saved #" << imageCount_R2 << endl;
				}
			}
			catch (GenICam::GenericException &e) {
				cout << "An error occurred saving the RIGHT2 image:" << endl;
				cout << e.GetDescription() << endl;
			}

			////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////
			cout << endl << endl;
		}
	}
	cout << "Program terminating..." << endl;
	system("PAUSE");
	return 0;
}

string getParamFromFile(string filepath, string pName) {
	//extracts using the format:
	//	pName1=Param1
	//	pName3=Param2
	//	pName2=Param3

	pName += "=";
	ifstream ifs(filepath.c_str());
	if (ifs.fail()) {
		cout << "Could not open specified file: " << filepath << endl;
		return "";
	}

	string tstring;
	while (!ifs.eof()) {
		getline(ifs, tstring);
		
		size_t found = tstring.find(pName);
		if (found != string::npos) {
			tstring = tstring.erase(found, pName.size());
			ifs.close();
			return tstring;
		}
	}
	return "";
}