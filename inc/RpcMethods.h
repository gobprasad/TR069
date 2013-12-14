#ifndef _RPC_METHODS_H_
#define _RPC_METHODS_H_
#include "XmlInterface.h"
#include <string>
#include <iostream>
#include "Database.h"
#include "WebClient.h"

class Inform : public XmlInterface {
	private:
		std::string GetCurrentTime();
		std::string GetRandomString();
		std::string GetEventListString();
		std::string GetInformValuesString();
		void CreateRpcString();
		DataBase db;
	public:
		Inform(const DataBase&, const WebClient&);
		~Inform();
		void InvokeService();
};

class SetParameterValue : public XmlInterface {
	private:
		DataBase db;
		void PerformSPVOperation();
		void CreateRpcString();
	public:
		SetParameterValue(const DataBase&, const WebClient&);
		virtual ~SetParameterValue();
		void InvokeService();
};


class GetParameterValue : public XmlInterface {
	private:
		DataBase db;
	public:
		GetParameterValue(const DataBase&, const WebClient&);
		virtual ~GetParameterValue();
		void InvokeService();
};

class EmptySend : public XmlInterface {
	private:
		DataBase db;
	public:
		EmptySend(const DataBase&, const WebClient&);
		virtual ~EmptySend();
		void InvokeService();
};

#endif
