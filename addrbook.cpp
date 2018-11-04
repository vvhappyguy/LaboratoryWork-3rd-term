#include <iostream>
#include <cstddef>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

#define FName "addbook1231.bin"
#define BYTE char*

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

int add(const char* cmd)
{
	// Steps:
	// (1) Validate command - code 1
	// (2) Create tmp_structure
	// (3) take pointer to endoffile (use fseek + spec.macro or sizeof file in bytes ??)
	// (4) open _ofs
	// (5) using (opened) sngltDB append new tmp_structure using ios::binary
	// (6) close _ofs
	// return 0  - successful

	// (1)
	size_t* words_counter = new size_t;
	(*words_counter) = 0;
	char* pars_cmd = new char[sizeof(cmd)];
	sprintf(pars_cmd,"%s",cmd);
	char* pch = strtok(pars_cmd," ");
	std::vector<std::string> words;
	while(pch != NULL)
	{
		*words_counter++;
		// std::cout << pch << std::endl;
		words.push_back(pch);
		pch = strtok(NULL," ");
		if(*words_counter > 8)
		{
			std::cout << "[ERR]: Bad command" << std::endl;
			delete words_counter;
			return 1; // Bad command
		}
	}
	delete pars_cmd;
	delete words_counter;
	
	Rec* tmp_rec = new Rec;
	strncpy(tmp_rec->LastName,words[0].c_str(),50);
	strncpy(tmp_rec->FirstName,words[1].c_str(),50);
	strncpy(tmp_rec->FatherName,words[2].c_str(),50);
	strncpy(tmp_rec->HB,words[3].c_str(),10);
	strncpy(tmp_rec->Address,words[4].c_str(),100);
	strncpy(tmp_rec->PhoneNumber,words[2].c_str(),20);
	strncpy(tmp_rec->Note,words[2].c_str(),4096);

	//	DB::Instance().getOF()->write((BYTE*)tmp_rec,sizeof(Rec));
	ofstream fo(FName,ios::binary);
	fo.seekp(ios::end);
	fo.write((BYTE)tmp_rec,sizeof(Rec));
	fo.close();


	return 0;
}

void parse_command(string command)
{
	const char* cmd = command.c_str();
	std::cout << "command:\t string: " << command << "\tc_str(): " << cmd << std::endl;
	if (strlen(cmd) > 0)
	{
		int result = 0;
		if (strncmp(cmd, "add",3) == 0)
		{
			cout << "Add /dev/null\n";
			result = add(cmd);
			if(result == 0)
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
		std::getline ( std::cin, command );
		cout << "Command: " << command << std::endl;
		parse_command(command);
	}

	return 0;
}
