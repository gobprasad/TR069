#include "WebClient.h"
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>

using namespace std;

string tempRecvBuf = "";

WebClient :: WebClient(){
	curl_global_init(CURL_GLOBAL_ALL);
	cout << "WEBCLIENT  STARTTED " << endl;
}

WebClient :: ~WebClient(){
	
}

void WebClient :: ClearWebClient(){
	curl_global_cleanup();
}

void WebClient :: StartSession(){
	this->recvString   = "";
	this->session      = false;
	this->acsUrl       = "";
	this->userName     = "";
	this->password     = "";
	this->localAddress = "";
	this->slist        = NULL;
	this->soapAction   = "";
	this->httpResponse = 0;
	this->handle       = curl_easy_init();
	this->slist        = curl_slist_append(this->slist, "Content-Type: text/xml; charset=utf-8");
}

void WebClient :: EndSession(){
	this->CleanRecvBuf();
	curl_easy_cleanup( this->handle );
}

void WebClient :: CleanRecvBuf(){
		this->recvString.clear();
}

void WebClient :: SetAcsUrl(string val){
	if( val != "" ){
		this->acsUrl = val;
	} else {
		throw AUEMT;
	}
}

void WebClient :: SetLocalAddress(string val){
	if( val != "" ){
		this->localAddress= val;
	} else {
		throw LAEMT;
	}
}

void WebClient :: SetUserName(string val){
	if( val != "" ){
		this->userName= val;
	} else {
		throw UNEMT;
	}
}

void WebClient :: SetSoapAction(string val){
	this->soapAction = val;
}

void WebClient :: SetUserPassword(string val){
	if( val != "" ){
		this->password= val;
	} else {
		throw PWEMT;
	}
}

void WebClient :: SetCurlOpt(){
	if( this->acsUrl   == "" ){
		throw VEMT;
		
	} else {

		curl_easy_setopt( this->handle, CURLOPT_POST, 1);
		//curl_easy_setopt( this->handle, CURLOPT_USERAGENT, "Gobind-TR069_client v1.0");
		curl_easy_setopt( this->handle, CURLOPT_USERAGENT, "gSoap 2.8.8 / Dimark client v4.3 []");
		curl_easy_setopt( this->handle, CURLOPT_URL, this->acsUrl.c_str());
		curl_easy_setopt( this->handle, CURLOPT_WRITEFUNCTION, WebClient :: RecvBufferCallback);
		curl_easy_setopt( this->handle, CURLOPT_COOKIEFILE, "");
		//curl_easy_setopt( this->handle, CURLOPT_WRITEDATA, this);
		if ( this->soapAction != "" ){
			string action = "soapAction: \"" + this->soapAction + "\"";
			this->slist = curl_slist_append(this->slist, action.c_str());
		}
		curl_easy_setopt( this->handle, CURLOPT_HTTPHEADER, this->slist);
	
#ifdef _CURLDEBUG_
		curl_easy_setopt( this->handle, CURLOPT_VERBOSE, 1);
#endif

#ifdef _LOCALIP_
		curl_easy_setopt( this->handle, CURLOPT_INTERFACE, this->localAddress.c_str());
#endif

#ifdef _WITHDIGEST_
		curl_easy_setopt( this->handle, CURLOPT_USERNAME, this->userName.c_str());
		curl_easy_setopt( this->handle, CURLOPT_PASSWORD, this->password.c_str());
		curl_easy_setopt( this->handle, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
#endif
	}
	
}

long WebClient :: GetHTTPResponseCode(){
	return this->httpResponse;
}

size_t WebClient:: RecvBufferCallback(void *contents, size_t size1, size_t nmemb, void *userp){
		size_t dataSize = size1*nmemb;
		tempRecvBuf += (char *)contents;
		return dataSize; 
}

void WebClient :: PostRequest(const char *sendBuf){
	curl_easy_setopt(this->handle, CURLOPT_POSTFIELDS, sendBuf);
	curl_easy_setopt( this->handle, CURLOPT_TCP_KEEPALIVE, 1);
	this->response = curl_easy_perform(this->handle);
	if(this->response != CURLE_OK){
		cout << "curl_easy_perform() failed: " << curl_easy_strerror(this->response) << endl;
		throw CERR;
	}
	curl_easy_getinfo(this->handle, CURLINFO_HTTP_CODE, &this->httpResponse);
	this->recvString = tempRecvBuf;
	tempRecvBuf.clear();
}
