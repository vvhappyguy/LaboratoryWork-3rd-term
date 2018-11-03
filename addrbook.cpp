#include <iostream>
#include <cstddef>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <string>

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
		fileDB.seekg(0, std::ios::end);
		_size = fileDB.tellg();
		fileDB.close();
		if (_size % sizeof(Rec) != 0)
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

	size_t getAddrN(size_t n)
	{
		return sizeof(Rec) * n;
	}

	void closeOF()
	{
		delete _ofs;
	}

	void closeIF()
	{
		delete _ifs;
	}

	const size_t getSize()
	{
		return _size;
	}

  private:
	ofstream* _ofs;
	ifstream* _ifs;
	size_t _size;
	DB(){};
	DB(const DB &root) = delete;
	DB &operator=(const DB &) = delete;
};

bool add()
{

}

void parse_command(string command)
{
	const char* cmd = command.c_str();
	if (strlen(cmd) > 0)
	{
		if (strncmp(cmd, "add",3) == 0)
		{
			cout << "Add /dev/null\n";
			return;
		}
		if (strncmp(cmd, "edit",4) == 0)
		{
			cout << "Edit /dev/null\n";
			return;
		}
		if (strncmp(cmd, "del",3) == 0)
		{
			cout << "Del /dev/null\n";
			return;
		}
		if (strncmp(cmd, "find",4) == 0)
		{
			cout << "Find /dev/null\n";
			return;
		}
		if (strncmp(cmd, "list",4) == 0)
		{
			cout << "List /dev/null\n";
			return;
		}
		if (strncmp(cmd, "count",5) == 0)
		{
			cout << "Count /dev/null\n";
			return;
		}
		cout << "Please read list of commands:" << std::endl
		 << "\tadd <...> \n\t- for adding new Rec to DB. \n\t\t(example: add Dyakin Ivan Pavlovich 09.09.1999 Moscow @riokin www.github.com/vvhappyguy)\n"
		 << "\tedit <n> <fieldname> <val> \n\t- for editting <fieldName> field of <n> Rec from db by your value <val> \n\t\t(example: edit 1 Address SergievPosad)\n"
		 << "\tdel <n> \n\t- for deletting <n> Rec from DB \n\t\t(example: del 2)\n";
	}	
	return;
}

int main()
{
	DB::Instance().connect();
	DB::Instance().getIF();
	std::string command;
	for (;;)
	{
		cout << "> ";
		cin >> command;
		cout << "Command: " << command << std::endl;
		parse_command(command);
	}

	return 0;
}
