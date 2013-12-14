#ifndef _DATABASE_H_
#define _DATABASE_H_
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <string>
using namespace std;
#define DFERR 0 // parameter file not able to load

class DataBase {
	private:
		xmlDocPtr doc;
		xmlXPathObjectPtr findNodeXpath(xmlChar *xpath);
	protected:
	public:
		DataBase();
		~DataBase();
		void ClearDataBase();
		void InitDataBase(const char *);
		void SetValue(string, string);
		string GetValue (string );
		string GetType (string );
		void GetInformParameters();
		void FactoryUpdate();
};

#endif
