#ifndef _WEB_CLIENT_H_
#define _WEB_CLIENT_H_

#include <iostream>
#include <curl/curl.h>

#define AUEMT    0    // AcsUrl is Empty
#define LAEMT    1    // local IP is Empty
#define UNEMT    2    // User Name is Empty
#define PWEMT    3    // Password is Empty
#define VEMT     4    // setOpt value Empty
#define CERR     5    // Curl Error



class WebClient {
private:
	CURL           *handle;
	CURLcode       response;
	long           httpResponse;
	bool           session;
	struct curl_slist *slist;
	std :: string  acsUrl;
	std :: string  localAddress;
	std :: string  userName;
	std :: string  password;
	std :: string soapAction;
	static size_t RecvBufferCallback(void *, size_t, size_t, void *);
protected:
public:
	      WebClient();
	 	  ~WebClient();  
	std :: string recvString;
	void     ClearWebClient();
	void     SetAcsUrl(std :: string);
	void     SetLocalAddress(std :: string);
	void     SetUserName(std :: string);
	void     SetUserPassword(std :: string);
	void     SetSoapAction(std :: string);
	void     SetCurlOpt();
	long     GetHTTPResponseCode();
	void     PostRequest( const char *);
	void     StartSession();
	void     EndSession();
	void     CleanRecvBuf();

};

#endif
