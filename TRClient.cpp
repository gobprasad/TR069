#include "TRClient.h"
#include "WebClient.h"
//#include "XmlInterface.h"
#include "Database.h"
#include "RpcMethods.h"
#include "Event.h"
#include <iostream>
#include <string>
#include <pthread.h>

#define DATA_MODEL "myDataModel.xml"

using namespace std;

pthread_mutex_t sessionLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  sessionGo   = PTHREAD_COND_INITIALIZER;

int main(){
	DataBase db;
	struct timespec timeToWait;
	try {
		db.InitDataBase(DATA_MODEL);
	} catch ( int e ){
		cout << "Error : " << e << endl;
		return 0;
	}
	db.FactoryUpdate();  // For Femto case

	//Testing only with BOOTSTRAP case TODO need to implement BOOT also
	AddEventSingle(EV_BOOTSTRAP);
	//-----------------------------------------------------------------
	int session = CONTINUE;
	int rpcName = INFORM;

	WebClient hClient; // Initiate WebClient

	while(1){   //Infinite loop for the process
		hClient.StartSession();
		hClient.SetAcsUrl(db.GetValue("InternetGatewayDevice.ManagementServer.URL"));
#ifdef _WITHDIGEST_
		hClient.SetUserName(db.GetValue("InternetGatewayDevice.ManagementServer.Username"));
		hClient.SetUserPassword(db.GetValue("InternetGatewayDevice.ManagementServer.Password"));
#endif
		hClient.SetCurlOpt();
		int emptyMarker = 0;
//--------------------------------------------------------------
		/*cout << "INFORM RPC" << endl;
					db.GetInformParameters();
					Inform *newRpc = new Inform(db, hClient);
					newRpc->InvokeService();
					rpcName = newRpc->nextRpc;
					delete newRpc;
					AddEventSingle(EV_BOOTSTRAP);
					rpcName = INFORM;*/
//-------------------------------------------------------------
	    pthread_mutex_lock(&sessionLock);
		while(session != END){
			switch(rpcName){
				case INFORM: {
					cout << "INFORM RPC" << endl;
					db.GetInformParameters();
					Inform *newRpc = new Inform(db, hClient);
					newRpc->InvokeService();
					rpcName = newRpc->nextRpc;
					delete newRpc;
					break;
				}
				case SPV: {
					SetParameterValue *newRpc = new SetParameterValue(db, hClient);
					newRpc->InvokeService();
					rpcName = newRpc->nextRpc;
					delete newRpc;
					break;
				}
				case GPV: {
					GetParameterValue *newRpc = new GetParameterValue(db, hClient);
					newRpc->InvokeService();
					rpcName = newRpc->nextRpc;
					delete newRpc;
					break;
				}
				case EMPTY: {
					cout << "EMPTY RPC" << endl;
					++emptyMarker;
					if(emptyMarker > 1){
						break;
					}
					EmptySend *newRpc = new EmptySend(db, hClient);
					cout << "We are sending Empty packet" << endl;
					newRpc->InvokeService();
					rpcName = newRpc->nextRpc;
					delete newRpc;
					break;
				}
				default : //TODO it should be Method not supported class
					cout << "Error on Rpc Name " << endl;
					//hClient.EndSession();
					exit(0);
					break;
			}
			if(emptyMarker > 1)
				break;				
		}
		session = END;
		hClient.EndSession();
		timeToWait.tv_sec = PeriodicWaitInterval(db);
		cout << "Next Periodic Inform Time  is : " <<  ctime(&timeToWait.tv_sec) << endl;
		timeToWait.tv_nsec = 0;
		if(timeToWait.tv_sec > 0 ){
			if (pthread_cond_timedwait(&sessionGo, &sessionLock, &timeToWait) == ETIMEDOUT){
				AddEventSingle(EV_PERIODIC);
			}
		} else {
			if (pthread_cond_wait ( &sessionGo, &sessionLock ) ){
				AddEventSingle(EV_PERIODIC);
			}
		}
		pthread_mutex_unlock(&sessionLock);
		AddEventSingle(EV_CR);
		
		session = CONTINUE;
		rpcName = INFORM;
		
		//TODO STOP THE SERVICE UNTILL SOMETHING HAPPENS		
	}
	db.ClearDataBase();
	hClient.ClearWebClient();
	
}

long PeriodicWaitInterval(DataBase& db){
	long informTime = -1;
	string tempInterval = db.GetValue("InternetGatewayDevice.ManagementServer.PeriodicInformInterval");
	string tempInformTime = db.GetValue("InternetGatewayDevice.ManagementServer.PeriodicInformTime");
	if( tempInformTime != "" ){
		informTime = StringToEpoch(tempInformTime);
	}
	long periodicIntervalDelay = strtol(tempInterval.c_str(),NULL,10);
    time_t now = time(NULL);
    cout << "Current time  is : " <<  ctime(&now) << endl;
    long diff = 0;
	
	if(informTime > 0 ){
		if( now > informTime ){
			diff = now - informTime;
			return ( now + (periodicIntervalDelay - ( diff % periodicIntervalDelay)));
		} else {
			diff = informTime - now;
			return (now + diff);
		}
	} else {
		return periodicIntervalDelay;
	}
}

long StringToEpoch(string dateString){
    char *p= (char *)dateString.c_str();
    int data[7],i=0;
    struct tm t;
    time_t t_of_day;
    do {
		if(isdigit(*p)){
            data[i] = (int)strtol(p,&p,0);
            i++;
        } else
            p++;
    }while(*p!= '\0');
    t.tm_year = data[0]-1900;
    t.tm_mon  = data[1]-1;           // Month, 0 - jan
    t.tm_mday = data[2];             // Day of the month
    t.tm_hour = data[3];
    t.tm_min  = data[4];
    t.tm_sec  = data[5];
    t.tm_isdst = -1;
    t_of_day  = mktime(&t);
    return ((long) t_of_day);
}