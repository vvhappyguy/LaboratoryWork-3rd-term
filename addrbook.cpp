#include <iostream>
#include <cstddef>
#include <algorithm>
#include <cstring>

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
	char* field(const char* Id);
};

struct FieldMap
{
	char   Id[20];
	size_t offset;
} fields [] = {
#define RegField(FIELD) { #FIELD , offsetof(Rec,  FIELD)}
	RegField(LastName),
	RegField(FirstName),
	RegField(FatherName),
	RegField(HB),
	RegField(Address),
	RegField(PhoneNumber),
	RegField(Note)
#if 0	
	{"F", offsetof(Rec, F)},
	{"I", offsetof(Rec, I)},
	{"O", offsetof(Rec, O)}
#endif	
#undef RegField
};
static const size_t FM_size = sizeof(fields)/sizeof(FieldMap);

char* Rec::field(const char* Id)
{
	FieldMap* fm_ptr = std::find_if(fields, fields+FM_size,
			[Id](const FieldMap& fm)->bool{return strcmp(Id, fm.Id) == 0;});

  return fm_ptr == fields+FM_size ? 
		NULL : reinterpret_cast<char*>(this) + fm_ptr->offset;
}

int main()
{

	Rec rec = {"Ivanov", "Ivan", "Ivanych","e","e","e","e"};

	char Id[20];

	while(true)
	{
		std::cout << "Id:" << std::flush; std::cin >> Id;
		char* field = rec.field(Id);
		if(field)		
			std::cout << "rec." << Id << " = " << field << std::endl;
		else
			std::cout << "No such field" << std::endl;
	}

	return 0;
}







