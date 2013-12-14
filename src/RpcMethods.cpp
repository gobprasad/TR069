#include "RpcMethods.h"
#include <cstdlib>
#include <string>
#include <ctime>
#include <sstream>
#include "Database.h"
#include "Event.h"
using namespace std;
extern list<int> eventList;
extern list<int> soapEventList;
extern list<ParamValueStruct> informList;
extern list<ParamValueStruct> soapInformList;
extern list<ParamValueStruct> soapRpcDataList;

/*-------------------------------------------------
	               Class Inform
--------------------------------------------------*/

Inform :: Inform(const DataBase& db, const WebClient& hClient)
		 : XmlInterface(hClient), db(db){
}

Inform :: ~Inform(){
}

/**
 *  Method for current Time
 *  Used in Inform RPC
 *--------------------------------------------------*/
string Inform :: GetCurrentTime(){
	string str = "";
	time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80] = {'\0'};
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%FT%T", &tstruct);
	str = buf;
    return str;
}

/**
 *  Method for Random String
 *  Used in Inform RPC for connection Request Url
 *--------------------------------------------------*/
string Inform :: GetRandomString(){
    int i;
    string str = "";
    srand (time(NULL));
    for(i=0; i<7; ++i){
		str += char(rand()%26 + 97);
    }
    return str;
}

/**
 *  Method for Event String
 *  Used in Inform RPC for Events
 *--------------------------------------------------*/
string Inform :: GetEventListString(){
	list<int>::iterator it;
	string str;
	str = "<Event xsi:type=\"soap:Array\" soap:arrayType=\"cwmp:EventStruct[";
	str += static_cast<ostringstream*>( &(ostringstream() << soapEventList.size()) )->str();
	str += "]\">";
	for (it=soapEventList.begin(); it!=soapEventList.end(); ++it){
		str += "<EventStruct><EventCode>" + GetEventString(*it) + "</EventCode><CommandKey></CommandKey></EventStruct>";
	}
	cout << "Event: " << GetEventString(*it) << endl;
	str += "</Event>";
  	return str;
}

/**
 *  Method for Inform Parameters
 *  Used in Inform RPC for Inform about the
 *  Parameters
 *--------------------------------------------------*/
string Inform :: GetInformValuesString(){
	string str = "";
	if( soapInformList.empty() ){
		return str;
	}
	list<ParamValueStruct>::iterator it;
	str = "<ParameterList xsi:type=\"soap:Array\" soap:arrayType=\"cwmp:ParameterValueStruct[";
	str += static_cast<ostringstream*>( &(ostringstream() << soapInformList.size()) )->str();
	str += "]\">";
	for (it=soapInformList.begin(); it!=soapInformList.end(); ++it){
		str += "<ParameterValueStruct><Name>" + it->name + "</Name><Value xsi:type=\"xsd:" + it->type + "\">";
		str += it->value + "</Value></ParameterValueStruct>";
	}
	str += "</ParameterList>";
	return str;
}

void Inform :: CreateRpcString(){
	this->rpcString  = "<cwmp:Inform><DeviceId>";
	this->rpcString += "<Manufacturer>"+ db.GetValue("InternetGatewayDevice.DeviceInfo.Manufacturer") + "</Manufacturer>"; 
	this->rpcString += "<OUI>" + db.GetValue("InternetGatewayDevice.DeviceInfo.ManufacturerOUI") + "</OUI>";
	this->rpcString += "<ProductClass>" + db.GetValue("InternetGatewayDevice.DeviceInfo.ProductClass") + "</ProductClass>";
	this->rpcString += "<SerialNumber>" + db.GetValue("InternetGatewayDevice.DeviceInfo.SerialNumber") + "</SerialNumber></DeviceId>";
	this->rpcString += GetEventListString();
	this->rpcString += "<MaxEnvelopes>1</MaxEnvelopes><CurrentTime>" + this->GetCurrentTime() + "</CurrentTime>";
}

void Inform :: InvokeService(){
	this->methodName = "Inform";
	int count = 0;
	int result = -1;
	CreateSoapEventList();
	AddInformData();
	while(result != 1){
		this->rpcString.clear();
		this->CreateRpcString();
		this->rpcString += "<RetryCount>" + static_cast<ostringstream*>( &(ostringstream() << count) )->str() +"</RetryCount>";
		this->rpcString += this->GetInformValuesString();
		this->rpcString += "</cwmp:Inform>";
		this->XmlSerialize();
		count++;
		result = 1;
		//TODO Exponential sleep is required here
	}
	void DeleteSoapEventList();
	void DeleteInformData();
}

SetParameterValue :: SetParameterValue(const DataBase& db, const WebClient& hClient)
		 : XmlInterface(hClient), db(db){
}

SetParameterValue :: ~SetParameterValue(){
}

void SetParameterValue :: PerformSPVOperation(){
	list<ParamValueStruct>::iterator it;
	for (it=soapRpcDataList.begin(); it!=soapRpcDataList.end(); ++it){
		db.SetValue(it->name,it->value);
	}
}

void SetParameterValue :: CreateRpcString(){
	this->rpcString = "<cwmp:SetParameterValuesResponse><Status>0</Status></cwmp:SetParameterValuesResponse>";
}

void SetParameterValue :: InvokeService(){
	this->methodName = "SetParameterValues";
	int count = 0;
	int result = -1;
	while(result != 1){
		this->rpcString.clear();
		this->PerformSPVOperation();
		this->CreateRpcString();
		this->XmlSerialize();
		count++;
		result = 1;
		//TODO Exponential sleep is required here
	}
	void DeleteRpcData();
}

GetParameterValue :: GetParameterValue(const DataBase& db, const WebClient& hClient)
		 : XmlInterface(hClient), db(db){
}

GetParameterValue :: ~GetParameterValue(){
}

void GetParameterValue :: InvokeService(){

}

//------------------ EmptySend -------------------------
EmptySend :: EmptySend(const DataBase& db, const WebClient& hClient)
		 : XmlInterface(hClient), db(db){
}

EmptySend :: ~EmptySend(){
}

void EmptySend :: InvokeService(){
	this->SendData();
}

