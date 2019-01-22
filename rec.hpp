#ifndef _REC_
#define _REC

#include <cstring>
#include <string>

using namespace std;

#pragma pack(push, 1)
class Rec
{
  public:
	char LastName[50];
	char FirstName[50];
	char FatherName[50];
	char HB[10];
	char Address[100];
	char PhoneNumber[20];
	char Note[4096];
	char *field(const char *Id);
};
#pragma pack(pop)

char *getFieldbyName(std::string field, Rec *rec)
{
	if (strcmp(field.c_str(), "FirstName") == 0)
		return rec->FirstName;
	else if (strcmp(field.c_str(), "LastName") == 0)
		return rec->LastName;
	else if (strcmp(field.c_str(), "FatherName") == 0)
		return rec->FatherName;
	else if (strcmp(field.c_str(), "HB") == 0)
		return rec->HB;
	else if (strcmp(field.c_str(), "Note") == 0)
		return rec->Note;
	else if (strcmp(field.c_str(), "Address") == 0)
		return rec->Address;
	else if (strcmp(field.c_str(), "PhoneNumber") == 0)
		return rec->PhoneNumber;
	else
		return nullptr;
}

#endif