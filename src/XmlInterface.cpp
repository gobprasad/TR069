#include "XmlInterface.h"
#include "WebClient.h"
#include "Event.h"
#include "TRClient.h"
#include <string>
#include <libxml/xpathInternals.h>

using namespace std;

XmlInterface :: XmlInterface(const WebClient& hClient) : mClient(hClient){
	this->InitXmlInterface();
}

XmlInterface :: ~XmlInterface(){
}

void XmlInterface :: InitXmlInterface(){
	this->sendBuf        = "";
	this->rpcString      = "";
	this->muSend         = "";
	this->muRecv         = "";
	this->methodName     = "";
	this->response       = false;
	this->nextRpc        = 0;
	this->errorCode      = 0;
}

void XmlInterface :: SetMustUnderstand(string val){
	this->muSend = val;
} 

string XmlInterface :: GetMustUnderstand(){
	return this->muSend;
}

void XmlInterface :: SendData(){
	long resCode = 0;
	cout << "------------Send ---------------------" << endl;
	cout << this->sendBuf << endl;
	mClient.PostRequest(this->sendBuf.c_str());
	resCode = mClient.GetHTTPResponseCode();
	cout << "------------Receive ---------------------" << resCode << endl;
	cout << mClient.recvString << endl;
	if(mClient.recvString.empty()){
		this->nextRpc = EMPTY;
		return;
	}
	this->XmlDeserialize(mClient.recvString);
}

void XmlInterface :: XmlSerialize (){
	this->sendBuf  = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	this->sendBuf += "<soapenv:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/encoding/\" ";
	this->sendBuf += "xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" ";
	this->sendBuf += "xmlns:cwmp=\"urn:dslforum-org:cwmp-1-0\" ";
	this->sendBuf += "xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" ";
	this->sendBuf += "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">";
	this->sendBuf += "<soapenv:Header><cwmp:ID soapenv:mustUnderstand=\"1\">" + GetMustUnderstand() + "</cwmp:ID></soapenv:Header>";
	this->sendBuf += "<soapenv:Body>";
	this->sendBuf += this->rpcString;
	this->sendBuf += "</soapenv:Body></soapenv:Envelope>";
	this->SendData();
}

/**
 * Parse Soap Response
 * Returns:
 *        0 OK (All went right)
 *        1 ERR_DATA (SOAP Response is not in correct xml Format)
 *        2 SOAP_ERROR (Sets errorCode Variable)
 *        3 MU_ERROR (MUST UNDERSTAND MISSMATCH)
 *        4 RES_ERROR (Response not received for last Request)
 *---------------------------------------------------*/

int XmlInterface :: XmlDeserialize(string& recvData){
	xmlDocPtr doc;
	doc = xmlParseMemory(recvData.c_str(), recvData.length());
	if( doc == NULL ){ //TODO need to check parser free
		return ERR_DATA;
	}
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;
	context = xmlXPathNewContext(doc); //TODO need to check for error (Internal Error)
	xmlXPathRegisterNs(context,  BAD_CAST "soap", BAD_CAST "http://schemas.xmlsoap.org/soap/encoding/");
	xmlXPathRegisterNs(context,  BAD_CAST "xsd", BAD_CAST "http://www.w3.org/2001/XMLSchema");
	xmlXPathRegisterNs(context,  BAD_CAST "cwmp", BAD_CAST "urn:dslforum-org:cwmp-1-0");
	xmlXPathRegisterNs(context,  BAD_CAST "xsi", BAD_CAST "http://www.w3.org/2001/XMLSchema-instance");
	xmlXPathRegisterNs(context,  BAD_CAST "soapenv", BAD_CAST "http://schemas.xmlsoap.org/soap/envelope/");

	string path;
/*Checking For ErrorCodes
-------------------------------------------------------------*/
	this->SoapErrorCheck(context, doc);
	if(this->errorCode){
		xmlXPathFreeContext(context);
		xmlFreeDoc(doc);
		xmlCleanupParser();
		return SOAP_ERROR;
	}
/*PARSING MUST UNDERSTAND VALUE
--------------------------------------------------------------*/
	path = "/soapenv:Envelope/soapenv:Header/cwmp:ID";
	result = FindAllNodes(context, path);
	if( result != NULL ){
		this->muRecv = this->GetStringOfNode(result->nodesetval->nodeTab[0],doc);
		xmlXPathFreeObject(result);
	} else {
		this->muRecv = "";
	}
	 // TODO need to check how to store mu from server for next rpc;
	if(!this->methodName.compare("Inform")){
		ParseInformResponse(context);
		this->nextRpc = EMPTY;
		xmlXPathFreeContext(context);
		xmlFreeDoc(doc);
		xmlCleanupParser();
		return OK;
	}
	this->CheckOtherRpc(context, doc);
	xmlXPathFreeContext(context);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return OK;
}


/**
 * GetStringOfNode()
 * Returns:
 *         Node name else empty string
 *---------------------------------------------------------------------*/
string XmlInterface :: GetStringOfNode(xmlNodePtr cur, xmlDocPtr doc ){
	xmlChar *temp;
	string result = "";
	temp = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
	if (temp == NULL ){
		return result;
	}
	result = (char *)temp;
	xmlFree(temp);
	return result;
}

/** 
 * FindAllNodes()
 * Finds all the nodes in xml of xpath
 * Returns:
 * 		 xmlPathObjectPtr which has all the nodes
 *----------------------------------------------------------------------*/

xmlXPathObjectPtr XmlInterface :: FindAllNodes(xmlXPathContextPtr context, string& path){
	xmlXPathObjectPtr result;
	result = xmlXPathEvalExpression((xmlChar *)path.c_str(), context);
	if(result == NULL){
		return NULL;
	} else if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
		xmlXPathFreeObject(result);
		return NULL;
	} else 
		return result;
}

/**
 * SoapErrorCheck()
 * Checks for soap error in response
 * Modify : errorCode variable of this class
 * ---------------------------------------------------*/
void XmlInterface :: SoapErrorCheck(xmlXPathContextPtr context, xmlDocPtr doc){
	string temp;
	this->errorCode = 0;
	string path = "/soapenv:Envelope/soapenv:Body/soapenv:Fault/detail/cwmp:Fault/FaultCode";
	xmlXPathObjectPtr result = this->FindAllNodes(context, path);
	if( result == NULL ){
		return;
	}
	temp = this->GetStringOfNode(result->nodesetval->nodeTab[0],doc);
	if(!temp.empty()){
		this->errorCode = strtol(temp.c_str(),NULL,10);
	}
	xmlXPathFreeObject(result);
}

/*PARSING RESPONSE FOR OUR REQUEST such as Inform
------------------------------------------------------------*/
void XmlInterface :: ParseInformResponse(xmlXPathContextPtr context){
	string path = "/soapenv:Envelope/soapenv:Body/cwmp:InformResponse";
	xmlXPathObjectPtr result = FindAllNodes(context, path);
	if( result != NULL ){
		this->response = true;
		xmlXPathFreeObject(result);
	} else {
		this->response = false;
	}
}

/*PARSING FOR OTHER RPC
----------------------------------------------------------*/
void XmlInterface :: CheckOtherRpc(xmlXPathContextPtr context, xmlDocPtr doc){
	string nextMeth;
	string path  = "/soapenv:Envelope/soapenv:Body/cwmp:SetParameterValues |";
	path += "cwmp:GetParameterValues | cwmp:SetParameterAttributes |";
	path += "cwmp:BOOT | cwmp:BOOTSTRAP";
	xmlXPathObjectPtr result = FindAllNodes(context, path);
	if( result != NULL ){
		nextMeth = (char *)result->nodesetval->nodeTab[0]->name;
		xmlXPathFreeObject(result);
	}
	if ( !nextMeth.compare("SetParameterValues")){
		this->nextRpc = SPV;
		this->GetNextRpcData(context, doc, "SetParameterValues");
	} else if(!nextMeth.compare("GetParameterValues")){
		this->nextRpc = GPV;
		this->GetNextRpcData(context, doc, "GetParameterValues");
	} else if(!nextMeth.compare("BOOT")){
		this->nextRpc = REBOOT;
	} else if(!nextMeth.compare("BOOTSTRAP")){
		this->nextRpc = BOOTSTRAP;
	} else {
		this->nextRpc = INVALID;
	}
	
}

void XmlInterface :: GetNextRpcData(xmlXPathContextPtr context, xmlDocPtr doc, string rpc){
	string name,value;
	string path = "/soapenv:Envelope/soapenv:Body/cwmp:"+ rpc + "/ParameterList/ParameterValueStruct";
	xmlXPathObjectPtr result = FindAllNodes(context, path);
	if(result == NULL)
		return;
	xmlNodeSetPtr nodeset;
	nodeset = result->nodesetval;
	xmlNodePtr cur;
	for ( int i=0; i < nodeset->nodeNr; i++) {
		cur = nodeset->nodeTab[i]->xmlChildrenNode;
		while(cur != NULL){
			if(!xmlStrcmp(cur->name,(const xmlChar *)"Name")){
				name = this->GetStringOfNode(cur,doc );
				cout << name << endl;
			}
			if(!xmlStrcmp(cur->name,(const xmlChar *)"Value")){
				value = this->GetStringOfNode(cur,doc );
				cout << value << endl;
			}
			cur = cur->next;
		}
		AddRpcData( name, "", value, false, false);
	}
	xmlXPathFreeObject (result);			
}
