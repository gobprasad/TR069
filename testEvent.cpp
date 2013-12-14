#include "Event.h"
#include <string>
#include <cstdlib>
#include <ctime>
#include <list>
#include <sstream>
#include <pthread.h>
using namespace std;

pthread_mutex_t eventMutex  = PTHREAD_MUTEX_INITIALIZER; //Event Mutex initialization
pthread_mutex_t pValueMutex = PTHREAD_MUTEX_INITIALIZER; //ParamValueStruct Mutex initialization

list<int> eventList;
list<int> soapEventList;
list<ParamValueStruct> InformList;
list<ParamValueStruct> soapParamList;
list<ParamValueStruct> soapRpcDataList;

// Returns Random string of length 7

string GetRandomString(){
    int i;
    string str = "";
    srand (time(NULL));
    for(i=0; i<7; ++i){
		str += char(rand()%26 + 97);
    }
    return str;
}

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
	soapEventList.splice (soapEventList.begin(), eventList);
	pthread_mutex_unlock(&eventMutex);
}

void DeleteSoapEventList(){
	soapEventList.clear();
}

string GetEventListString(){
	list<int>::iterator it;
	string str;
	str = "<Event xsi:type=\"soap:Array\" soap:arrayType=\"cwmp:EventStruct[";
	str += static_cast<ostringstream*>( &(ostringstream() << soapEventList.size()) )->str();
	str += "]\">";
	for (it=soapEventList.begin(); it!=soapEventList.end(); ++it){
		str += "<EventStruct><EventCode>" + GetEventString(*it) + "</EventCode><CommandKey></CommandKey></EventStruct>";
	}
	str += "</Event>";
  	return str;
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
	paramList.push_back(ParamValueStruct(name,type,value,false,false));
	pthread_mutex_unlock(&pValueMutex);
}


void CreateSoapParamValue(){
	pthread_mutex_lock(&pValueMutex);
	soapParamList.splice (soapParamList.begin(), paramList);
	pthread_mutex_unlock(&pValueMutex);
}

string GetParameterValueStruct(){
	string str = "";
	if( soapParamList.empty() ){
		return str;
	}
	list<ParamValueStruct>::iterator it;
	str = "<ParameterList xsi:type=\"soap:Array\" soap:arrayType=\"cwmp:ParameterValueStruct[";
	str += static_cast<ostringstream*>( &(ostringstream() << soapParamList.size()) )->str();
	str += "]\">";
	for (it=soapParamList.begin(); it!=soapParamList.end(); ++it){
		str += "<ParameterValueStruct><Name>" + it->name + "</Name><Value xsi:type=\"xsd:" + it->type + "\">";
		str += it->value + "</Value></ParameterValueStruct>";
	}
	str += "</ParameterList>";
	return str;
}

void DeleteSoapParamList(){
	soapParamList.clear();
}

void AddRpcData( string name, string type, string value, bool notify, bool access){
	soapRpcDataList.push_back(ParamValueStruct(name,type,value, notify, access));
}

void DeleteRpcData(){
	soapRpcDataList.clear();
}
