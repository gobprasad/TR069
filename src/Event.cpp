#include "Event.h"
#include <string>
#include <cstdlib>
#include <ctime>
#include <list>
#include <pthread.h>
using namespace std;

list<int> eventList;
list<int> soapEventList;
list<ParamValueStruct> informList;
list<ParamValueStruct> soapInformList;
list<ParamValueStruct> soapRpcDataList;

pthread_mutex_t eventMutex  = PTHREAD_MUTEX_INITIALIZER; //Event Mutex initialization
pthread_mutex_t pValueMutex = PTHREAD_MUTEX_INITIALIZER; //ParamValueStruct Mutex initialization

// EVENT LIST API's
//--------------------------------------------------------
void AddEventSingle(int val){
	pthread_mutex_lock(&eventMutex);
	eventList.push_back(val);
	eventList.unique();
	pthread_mutex_unlock(&eventMutex);
}

void CreateSoapEventList(){
	pthread_mutex_lock(&eventMutex);
	soapEventList.splice(soapEventList.begin(), eventList);
	pthread_mutex_unlock(&eventMutex);
}

void DeleteSoapEventList(){
	soapEventList.clear();
}

string GetEventString(int val){
	switch(val){
		case 0:
			return "0 BOOTSTRAP";
			break;
		case 1:
			return "1 BOOT";
			break;
		case 2:
			return "2 PERIODIC";
			break;
		case 3:
			return "3 SCHEDULED";
			break;
		case 4:
			return "4 VALUE CHANGE";
			break;
		case 5:
			return "5 KICKED";
			break;
		case 6:
			return "6 CONNECTION REQUEST";
			break;
		case 7:
			return "7 TRANSFER COMPLETE";
			break;
		case 8:
			return "8 DIAGNOSTICS COMPLETE";
			break;
		case 9:
			return "9 REQUEST DOWNLOAD";
			break;
		case 10:
			return "10 AUTONOMOUS TRANSFER COMPLETE";
			break;
		case 11:
			return "M Reboot";
			break;
		case 12:
			return "M Scheduled Inform";
			break;
		case 13:
			return "M Download";
			break;
		case 14:
			return "M Upload";
			break;
		default:
			return "";
			break;
	}
}

//PARAM VALUE API's
//---------------------------------------------------------------

void AddParamValueStruct( string name, string type, string value ){
	pthread_mutex_lock(&pValueMutex);
	informList.push_back(ParamValueStruct(name,type,value,false, false));
	pthread_mutex_unlock(&pValueMutex);
}


void AddInformData(){
	pthread_mutex_lock(&pValueMutex);
	soapInformList.splice (soapInformList.begin(), informList);
	pthread_mutex_unlock(&pValueMutex);
}

void DeleteInformData(){
	soapInformList.clear();
}

void AddRpcData( string name, string type, string value, bool notify, bool access){
	soapRpcDataList.push_back(ParamValueStruct(name,type,value, notify, access));
}

void DeleteRpcData(){
	soapRpcDataList.clear();
}
