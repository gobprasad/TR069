#ifndef _EVENT_H_
#define _EVENT_H_
#include <iostream>
#include <string>
#include <list>
using namespace std;

#define EV_BOOTSTRAP    0   // "0 BOOTSTRAP"
#define EV_BOOT         1   // "1 BOOT"
#define EV_PERIODIC     2   // "2 PERIODIC"
#define EV_SCHEDULED    3   // "3 SCHEDULED"
#define EV_VC           4   // "4 VALUE CHANGE"
#define EV_KICKED       5   // "5 KICKED"
#define EV_CR           6   // "6 CONNECTION REQUEST"
#define EV_TC           7   // "7 TRANSFER COMPLETE"
#define EV_DC           8   // "8 DIAGNOSTICS COMPLETE"
#define EV_RD           9   // "9 REQUEST DOWNLOAD"
#define EV_ATC          10  // "10 AUTONOMOUS TRANSFER COMPLETE"
#define EV_MBOOT        11  // "M Reboot"
#define EV_MSI          12  // "M Scheduled Inform"
#define EV_MDOWNLOAD    13  // "M Download"
#define EV_MUPLOAD      14  // "M Upload"

class ParamValueStruct {
	private:
	protected:
	public:
		ParamValueStruct(string name, string type, string val, bool notify, bool access){
			this->name = name;
			this->type = type;
			this->value = val;
			this->notification = notify;
			this->accessList = access;

		}
		virtual ~ParamValueStruct(){}
		string name;
		string type;
		string value;
		bool notification;
		bool accessList;
};

void AddEventSingle(int);
void CreateSoapEventList();
void DeleteSoapEventList();
string GetEventString(int);

void AddParamValueStruct( string, string, string);
void AddInformData();
void DeleteInformData();
void AddRpcData( string, string, string, bool, bool);
void DeleteRpcData();

#endif
