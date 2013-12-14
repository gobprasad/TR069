#include "Database.h"
#include "Event.h"
#include <libxml/xpath.h>
#include <libxml/parser.h>
#include <libxml/xpathInternals.h>
#include <string>
#include <iostream>
using namespace std;

DataBase :: DataBase(){
	cout << "DATABASE CONSTRUCTOR" << endl;
}

DataBase :: ~DataBase(){
}

void DataBase :: InitDataBase(const char *fileName){
	if ((this->doc = xmlParseFile(fileName)) == NULL )
		throw DFERR;
}

void DataBase :: ClearDataBase(){
	xmlCleanupParser();
	xmlFreeDoc(doc);
}

xmlXPathObjectPtr DataBase :: findNodeXpath(xmlChar *xpath){
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;
	context = xmlXPathNewContext(this->doc);
	xmlXPathRegisterNs(context,  BAD_CAST "dm", BAD_CAST "urn:broadband-forum-org:cwmp:datamodel-1-2");
	result = xmlXPathEvalExpression(xpath, context);
	xmlXPathFreeContext(context);
	if (result == NULL) {
		return NULL;
	}
	if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
		xmlXPathFreeObject(result);
		return NULL;
	}
	return result;
}


string DataBase :: GetValue (string mName ){
	string xpath, value = "";
	unsigned found = mName.find_last_of(".");
	string path    = mName.substr(0,found+1);
	string name    = mName.substr(found+1);
	if ( name != "" )
		xpath   = "/dm:document/model/object[@base='" + path + "']/parameter[@base='" + name + "']/syntax/default[1]";
	else
		xpath   = "/dm:document/model/object[contains(@base,'" + path + "')]/parameter/syntax/default";
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;
	result = findNodeXpath( (xmlChar *)xpath.c_str() );
	xmlChar *tempValue;

	if (result) {
		nodeset = result->nodesetval;
		for ( int i=0; i < nodeset->nodeNr; i++) {
			tempValue = xmlGetProp(nodeset->nodeTab[i], (const xmlChar *)"value");
			value = (char *)tempValue;
			xmlFree(tempValue);
		}
		xmlXPathFreeObject (result);
	}else {
		value = "NONE";
	}
	return value;
}


void DataBase :: SetValue(string mName, string value){
	unsigned found = mName.find_last_of(".");
	string path    = mName.substr(0,found+1);
	string name    = mName.substr(found+1);
	string xpath   = "/dm:document/model/object[@base='" + path + "']/parameter[@base='" + name + "']/syntax/default[1]";
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;
	result = findNodeXpath( (xmlChar *)xpath.c_str() );
	if (result) {
		nodeset = result->nodesetval;
		for ( int i=0; i < nodeset->nodeNr; i++) {
			xmlSetProp(nodeset->nodeTab[i], (const xmlChar *)"value",(const xmlChar *)value.c_str());
		}
		xmlXPathFreeObject (result);
	}
}

string DataBase :: GetType(string paramName){
	unsigned found = paramName.find_last_of(".");
	string path    = paramName.substr(0,found+1);
	string name    = paramName.substr(found+1);
	string xpath   = "/dm:document/model/object[@base='" + path + "']/parameter[@base='" + name + "']/syntax[1]";
	xmlXPathObjectPtr result;
	result = findNodeXpath( (xmlChar *)xpath.c_str() );
	xmlNodePtr cur;
	if (result) {
		cur = result->nodesetval->nodeTab[0]->xmlChildrenNode;
		while(cur != NULL){
			if ((!xmlStrcmp(cur->name, (const xmlChar *)"text")))
				cur = cur->next;
			else
				break;
		}
		xmlXPathFreeObject (result);
	}
	return (char *)cur->name;
}

void DataBase :: GetInformParameters(){
	string xpath = "/dm:document/model/object/parameter[@notification=3]";
	string paramName = "";
	string value = "";
	string type = "";
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;
	xmlChar *tempValue;
	result = findNodeXpath( (xmlChar *)xpath.c_str() );
	if (result) {
		nodeset = result->nodesetval;
		for ( int i=0; i < nodeset->nodeNr; i++) {
			tempValue = xmlGetProp(nodeset->nodeTab[i]->parent, (const xmlChar *)"base");
			paramName = (char *)tempValue;
			xmlFree(tempValue);
			tempValue = xmlGetProp(nodeset->nodeTab[i], (const xmlChar *)"base");
			paramName += (char *)tempValue;
			xmlFree(tempValue);
			value = this->GetValue(paramName);
			type  = this->GetType(paramName);
			//cout << paramName << " : " << type << " : " << value << endl;
			AddParamValueStruct(paramName, type, value);
			paramName = "";
			type = "";
			value ="";
		}
		xmlXPathFreeObject (result);
	}else {
		throw 9002;
	}
}

void DataBase :: FactoryUpdate(){
	this->SetValue("InternetGatewayDevice.ManagementServer.ParameterKey", "NONE");
	this->SetValue("InternetGatewayDevice.ManagementServer.ConnectionRequestURL", "http://172.17.16.59/acsCall");
	this->SetValue("InternetGatewayDevice.DeviceInfo.ProductClass" ,"cdmaFAP2");
	this->SetValue("InternetGatewayDevice.DeviceInfo.SerialNumber" ,"gtest0001");
	this->SetValue("InternetGatewayDevice.DeviceInfo.HardwareVersion" ,"1.0");
	this->SetValue("InternetGatewayDevice.DeviceInfo.SoftwareVersion", "6.5.00.102");
	this->SetValue("InternetGatewayDevice.DeviceInfo.SpecVersion", "1.0");
	this->SetValue("InternetGatewayDevice.DeviceInfo.ProvisioningCode", "NONE");
	this->SetValue("InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.ExternalIPAddress", "172.17.16.59");
	this->SetValue("InternetGatewayDevice.Services.X_0005B9_DOMO.X_0005B9_is856SectorElement.sectorElementAdminStatus", "1");
	this->SetValue("InternetGatewayDevice.Services.X_0005B9_DOMO.X_0005B9_is856SectorElement.sectorElementOperStatus" ,"1");
	this->SetValue("InternetGatewayDevice.Services.X_0005B9_OneXMO.X_0005B9_OneXRTTMO.AdminState", "0");
	this->SetValue("InternetGatewayDevice.Services.X_0005B9_OneXMO.X_0005B9_OneXSectorMO.OneXSectorAdminState","lock");
	this->SetValue("InternetGatewayDevice.Services.X_0005B9_OneXMO.X_0005B9_OneXSectorMO.OneXSectorOperState", "1");
	this->SetValue("InternetGatewayDevice.Services.X_0005B9_OneXMO.X_0005B9_OneXBeaconMO.beaconAdminStatus","0");
	this->SetValue("InternetGatewayDevice.ManagementServer.URL", "http://10.204.4.27:7003/cwmpWeb/DigestCPEMgt?target=/CPEMgt");
	this->SetValue("InternetGatewayDevice.ManagementServer.Username","gtest0001-01");
	this->SetValue("InternetGatewayDevice.ManagementServer.Password","gtest0001-01");
}
