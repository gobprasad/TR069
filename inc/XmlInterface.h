#ifndef _XMLINTERFACE_H_
#define _XMLINTERFACE_H_
#include "Database.h"
#include "WebClient.h"
#include <string>
using namespace std;

#define OK          0   //(All went right)
#define ERR_DATA    1   //(SOAP Response is not in correct xml Format)
#define SOAP_ERROR  2   //(Sets errorCode Variable)
#define MU_ERROR    3   //(MUST UNDERSTAND MISSMATCH)
#define RES_ERROR   4   //(Response not received for last Request)

class XmlInterface {
private:
	WebClient mClient;
	std :: string GetStringOfNode(xmlNodePtr, xmlDocPtr );
	xmlXPathObjectPtr FindAllNodes(xmlXPathContextPtr, std ::string&);
	void SoapErrorCheck(xmlXPathContextPtr, xmlDocPtr);
	void ParseInformResponse(xmlXPathContextPtr);
	void CheckOtherRpc(xmlXPathContextPtr, xmlDocPtr);
	void GetNextRpcData(xmlXPathContextPtr, xmlDocPtr, string);
protected:
public:
			 XmlInterface(const WebClient&);
	~XmlInterface();
	bool response;
	long int errorCode;
	std :: string sendBuf;
	std :: string rpcString;
	std :: string muSend;
	std :: string muRecv;
	std :: string methodName;
	int nextRpc;
	void SetMustUnderstand(std::string);
	std :: string GetMustUnderstand();
	void InitXmlInterface();
	void XmlSerialize();
	int XmlDeserialize(std::string&);
	void SendData();
};

#endif
