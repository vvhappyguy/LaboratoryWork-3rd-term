#include <iostream>
#include <cstddef>
#include <algorithm>
#include <cstring>
#include <fstream>

#define FName "addbook1231.bin"

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

struct FieldMap
{
	char Id[20];
	size_t offset;
} fields[] = {
#define RegField(FIELD)              \
	{                                \
#FIELD, offsetof(Rec, FIELD) \
	}
	RegField(LastName),
	RegField(FirstName),
	RegField(FatherName),
	RegField(HB),
	RegField(Address),
	RegField(PhoneNumber),
	RegField(Note)
#if 0	
		{"LastName", offsetof(Rec, LastName)},
		{"FirstName", offsetof(Rec, FirstName)},
		{"FatherName", offsetof(Rec, FatherName)},
		{"HB", offsetof(Rec, HB)},
		{"Address", offsetof(Rec, Address)},
		{"PhoneNumber", offsetof(Rec, PhoneNumber)},
		{"Note", offsetof(Rec, Note)}
#endif
#undef RegField
};

static const size_t FM_size = sizeof(fields) / sizeof(FieldMap);

char *Rec::field(const char *Id)
{
	FieldMap *fm_ptr = std::find_if(fields, fields + FM_size,
									[Id](const FieldMap &fm) -> bool { return strcmp(Id, fm.Id) == 0; });

	return fm_ptr == fields + FM_size ? NULL : reinterpret_cast<char *>(this) + fm_ptr->offset;
}

class DB
{
  public:
	static DB &Instance()
	{
		static DB theSingleInstance;
		return theSingleInstance;
	}

	void connect()
	{
		
		if (!fstream(FName).good())
		{
			cout << "Do you want to create new DB by name: " << FName << " (yes/no)" << std::endl;
			char *ans = new char[3];
			cin >> ans;
			if (strcmp(ans,"yes"))
			{
				delete ans;
				throw runtime_error("Bad filename of DB");
			}

			else
			{
				delete ans;
				ofstream ofs(FName); 
				ofs.close();
			}
		}
		fstream fileDB(FName);
		size_t size = 0;
		fileDB.seekg(0, std::ios::end);
		size = fileDB.tellg();
		fileDB.close();
		if (size % sizeof(Rec) != 0)
		{
			throw runtime_error("Bad structure of DB file");
		}
	}

	ofstream* getOF()
	{
		if(!_ofs)
			_ofs = new ofstream(FName);
		return _ofs;
	}

	ifstream* getIF()
	{
		if(_ifs)
			_ifs = new ifstream(FName);
		return _ifs;
	}

	void closeOF()
	{
		delete _ofs;
	}

	void closeIF()
	{
		delete _ifs;
	}

  private:
	ofstream* _ofs;
	ifstream* _ifs;
	DB(){};
	DB(const DB &root) = delete;
	DB &operator=(const DB &) = delete;
};

void parse_command(const char *command)
{
	if (strlen(command) > 0)
	{
		if (strcmp(command, "add") == 0)
		{
			cout << "Add /dev/null\n";
			return;
		}
		if (strcmp(command, "edit") == 0)
		{
			cout << "Edit /dev/null\n";
			return;
		}
		if (strcmp(command, "del") == 0)
		{
			cout << "Del /dev/null\n";
			return;
		}
		if (strcmp(command, "find") == 0)
		{
			cout << "Find /dev/null\n";
			return;
		}
		if (strcmp(command, "list") == 0)
		{
			cout << "List /dev/null\n";
			return;
		}
		if (strcmp(command, "count") == 0)
		{
			cout << "Count /dev/null\n";
			return;
		}
	}
	cout << "Please read list of commands:" << std::endl
		 << "\tadd <...> \n\t- for adding new Rec to DB. \n\t\t(example: add Dyakin Ivan Pavlovich 09.09.1999 Moscow @riokin www.github.com/vvhappyguy)\n"
		 << "\tedit <n> <fieldname> <val> \n\t- for editting <fieldName> field of <n> Rec from db by your value <val> \n\t\t(example: edit 1 Address SergievPosad)\n"
		 << "\tdel <n> \n\t- for deletting <n> Rec from DB \n\t\t(example: del 2)\n";
	return;
}

int main()
{
	DB::Instance().connect();
	DB::Instance().getIF();

	char command[50] = "";
	for (;;)
	{
		cout << "> ";
		cin >> command;
		parse_command(command);
	}

	return 0;
}
