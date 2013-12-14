#ifndef _TRCLIENT_H_
#define _TRCLIENT_H_
#include "Database.h"
#include <string>

#define CONTINUE   1
#define END        0
#define INFORM     100
#define SPV        101
#define GPV        102
#define EMPTY      103
#define REBOOT     104
#define BOOTSTRAP  105
#define INVALID    106

long PeriodicWaitInterval(DataBase&);
long StringToEpoch(std::string);

#endif
