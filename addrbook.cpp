#include <iostream>
#include <cstddef>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <cstring>
#include <vector>
#include <unistd.h>

#define FName "addbook.bin"
#define BYTE char *

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
	size_t _size;
	size_t count;
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
			if (strcmp(ans, "yes"))
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

		this->count = _size / sizeof(Rec);
		fileDB.close();
		if (_size % sizeof(Rec) != 0)
		{
			throw runtime_error("Bad structure of DB file");
		}
	}

	ofstream *getOF()
	{
		if (!_ofs)
			_ofs = new ofstream(FName);
		return _ofs;
	}

	ifstream *getIF()
	{
		if (_ifs)
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
		return this->Instance()._size;
	}

	void sizeP()
	{
		this->_size++;
	}

	void sizeM()
	{
		this->_size--;
	}

  private:
	ofstream *_ofs;
	ifstream *_ifs;

	DB(){};
	DB(const DB &root) = delete;
	DB &operator=(const DB &) = delete;
};

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

int add(const char *cmd)
{
	size_t words_counter = 0;
	char *pars_cmd = new char[strlen(cmd)];
	sprintf(pars_cmd, "%s", cmd);
	char *pch = strtok(pars_cmd, " ");
	std::vector<std::string> words;
	while (pch != NULL)
	{

		std::cout << "pch = " << pch
				  << "\twords.size() = " << words.size()
				  << "\tcounter: " << words_counter << std::endl;
		words.push_back(pch);
		words_counter++;
		pch = strtok(NULL, " ");
		if (words_counter > 8)
		{
			std::cout << "[ERR]: Bad command" << std::endl;
			delete pars_cmd;
			return 1; // Bad command
		}
	}
	std::cout << "DBG1" << std::endl;
	delete pars_cmd;
	if (words.size() != 8)
	{
		cout << "Words.size() = " << words.size() << "\n";
		std::cout << "[ERR]: Bad command" << std::endl;
		return 1;
	}

	std::cout << "Creating new Rec" << std::endl;
	Rec *tmp_rec = new Rec;
	strncpy(tmp_rec->LastName, words[1].c_str(), 50);
	strncpy(tmp_rec->FirstName, words[2].c_str(), 50);
	strncpy(tmp_rec->FatherName, words[3].c_str(), 50);
	strncpy(tmp_rec->HB, words[4].c_str(), 10);
	strncpy(tmp_rec->Address, words[5].c_str(), 100);
	strncpy(tmp_rec->PhoneNumber, words[6].c_str(), 20);
	strncpy(tmp_rec->Note, words[7].c_str(), 4096);

	//	DB::Instance().getOF()->write((BYTE*)tmp_rec,sizeof(Rec));
	ofstream fo(FName, ios::binary | std::ios::app);
	fo.seekp((DB::Instance().count) * sizeof(Rec), std::ifstream::cur);
	fo.write((BYTE)tmp_rec, sizeof(Rec));
	fo.close();
	DB::Instance().count++;
	delete tmp_rec;
	return 0;
}

int list(const char *cmd)
{
	if (DB::Instance().count == 0)
		return 1;
	size_t words_counter = 0;
	char *pars_cmd = new char[strlen(cmd)];
	sprintf(pars_cmd, "%s", cmd);
	char *pch = strtok(pars_cmd, " ");
	std::vector<std::string> words;
	while (pch != NULL)
	{

		std::cout << "pch = " << pch
				  << "\twords.size() = " << words.size()
				  << "\tcounter: " << words_counter << std::endl;
		words.push_back(pch);
		words_counter++;
		pch = strtok(NULL, " ");
		if (words_counter > 3)
		{
			std::cout << "[ERR]: Bad command" << std::endl;
			delete pars_cmd;
			return 1; // Bad command
		}
	}
	std::cout << "DBG1dasd" << std::endl;
	delete pars_cmd;
	if (words.size() > 3)
	{
		cout << "Words.size() = " << words.size() << "\n";
		std::cout << "[ERR]: Bad command" << std::endl;
		return 1;
	}

	size_t i1, i2; // Two counters for limits

	if (words.size() == 1)
	{
		i1 = 0;
		i2 = DB::Instance().count;
	}
	else if (words.size() == 2)
	{
		i1 = stoul(words[1]);
		i2 = DB::Instance().count;
	}
	else if (words.size() == 3)
	{
		i1 = stoul(words[1]);
		i2 = stoul(words[2]);
	}
	Rec cout_rec;
	ifstream f2(FName, ios::binary | ios::in);
	std::cout << i1 << " " << i2 << std::endl;
	for (size_t i = 1; i <= DB::Instance().count; i++)
	{
		f2.read((BYTE)&cout_rec, sizeof(Rec));
		if ((i >= i1) && (i <= i2))
			std::cout << "Number [" << i << "]: "
					  << cout_rec.LastName << " " << cout_rec.FirstName << " "
					  << cout_rec.FatherName << " " << cout_rec.HB << " "
					  << cout_rec.Address << " " << cout_rec.PhoneNumber << " "
					  << cout_rec.Note << std::endl;
	}
	return 0;
}

int edit(const char *cmd)
{
	size_t words_counter = 0;
	char *pars_cmd = new char[strlen(cmd)];
	sprintf(pars_cmd, "%s", cmd);
	char *pch = strtok(pars_cmd, " ");
	std::vector<std::string> words;
	while (pch != NULL)
	{

		std::cout << "pch = " << pch
				  << "\twords.size() = " << words.size()
				  << "\tcounter: " << words_counter << std::endl;
		words.push_back(pch);
		words_counter++;
		pch = strtok(NULL, " ");
		if (words_counter > 4)
		{
			std::cout << "[ERR]: Bad command" << std::endl;
			delete pars_cmd;
			return 1; // Bad command
		}
	}
	delete pars_cmd;
	delete pch;

	size_t n = stoul(words[1].c_str());
	Rec *tmp_rec = new Rec();
	fstream f2(FName, ios::in | ios::out | ios::binary);
	f2.seekg((n - 1) * sizeof(Rec), std::ifstream::cur);
	f2.read((BYTE)tmp_rec, sizeof(Rec));

	// std::cout << "EDIT [" << n << "]: "
	// 				  << tmp_rec->LastName << " " << tmp_rec->FirstName << " "
	// 				  << tmp_rec->FatherName << " " << tmp_rec->HB << " "
	// 				  << tmp_rec->Address << " " << tmp_rec->PhoneNumber << " "
	// 				  << tmp_rec->Note << std::endl;

	//char* tmp = tmp_rec->field(words[2].c_str());
	char *tmp = getFieldbyName(words[2], tmp_rec);
	//std::cout << "Field: " << tmp << std::endl;
	sprintf(tmp, "%s", words[3].c_str());
	f2.seekp((n - 1) * sizeof(Rec), ios::beg);
	f2.write((BYTE)tmp_rec, sizeof(Rec));
	f2.close();
	delete tmp_rec;
	return 0;
}

int del(const char *cmd)
{
	size_t words_counter = 0;
	char *pars_cmd = new char[strlen(cmd)];
	sprintf(pars_cmd, "%s", cmd);
	char *pch = strtok(pars_cmd, " ");
	std::vector<std::string> words;
	while (pch != NULL)
	{

		std::cout << "pch = " << pch
				  << "\twords.size() = " << words.size()
				  << "\tcounter: " << words_counter << std::endl;
		words.push_back(pch);
		words_counter++;
		pch = strtok(NULL, " ");
		if (words_counter > 2)
		{
			std::cout << "[ERR]: Bad command" << std::endl;
			delete pars_cmd;
			return 1; // Bad command
		}
	}
	delete pars_cmd;
	delete pch;

	size_t n = stoul(words[1].c_str());
	Rec *tmp_rec_del = new Rec();
	Rec *tmp_rec_last = new Rec();
	fstream f2(FName, ios::in | ios::out | ios::binary);
	f2.seekg((n - 1) * sizeof(Rec), ios::beg);
	f2.read((BYTE)tmp_rec_del, sizeof(Rec));
	f2.seekg((DB::Instance().count - 1) * sizeof(Rec), ios::beg);
	f2.read((BYTE)tmp_rec_last, sizeof(Rec));
	f2.seekp((n - 1) * sizeof(Rec), ios::beg);
	f2.write((BYTE)tmp_rec_last, sizeof(Rec));
	f2.seekg((DB::Instance().count - 1) * sizeof(Rec), ios::beg);
	truncate(FName, sizeof(Rec) * (DB::Instance().count - 1));
	DB::Instance().count--;
	f2.close();
	delete tmp_rec_del;
	delete tmp_rec_last;
	return 0;
}

int find(const char *cmd)
{
	size_t words_counter = 0;
	char *pars_cmd = new char[strlen(cmd)];
	sprintf(pars_cmd, "%s", cmd);
	char *pch = strtok(pars_cmd, " ");
	std::vector<std::string> words;
	while (pch != NULL)
	{

		std::cout << "pch = " << pch
				  << "\twords.size() = " << words.size()
				  << "\tcounter: " << words_counter << std::endl;
		words.push_back(pch);
		words_counter++;
		pch = strtok(NULL, " ");
		if (words_counter > 3)
		{
			std::cout << "[ERR]: Bad command" << std::endl;
			delete pars_cmd;
			return 1; // Bad command
		}
	}
	delete pars_cmd;
	delete pch;

	Rec cout_rec;
	ushort num = 0;
	ifstream f2(FName, ios::binary | ios::in);
	for (size_t i = 1; i <= DB::Instance().count; i++)
	{
		f2.read((BYTE)&cout_rec, sizeof(Rec));
		char *tmp = getFieldbyName(words[1], &cout_rec);
		std::cout << tmp << " " << words[2].c_str() << std::endl;
		if (strncmp(tmp, words[2].c_str(), words[2].size()) == 0)
		{
			num = i;
			break;
		}
		if (i == DB::Instance().count)
			num = 0;
	}
	f2.close();
	if (num != 0)
		std::cout << "Find your field[" << num << "]" << std::endl;
	else
		std::cout << "No such field!" << std::endl;

	return 0;
}

void parse_command(string command)
{
	std::cout << "[" << DB::Instance().count << "]command:\t string: " << command << std::endl;
	if (strlen(command.c_str()) > 0)
	{
		int result = 0;
		if (strncmp(command.c_str(), "add", 3) == 0)
		{
			cout << "Add\n";
			result = add(command.c_str());
			if (result == 0)
				return;
		}
		else if (strncmp(command.c_str(), "count", 5) == 0)
		{
			cout << "Count: " << DB::Instance().count << std::endl;
			return;
		}
		else if (DB::Instance().count == 0)
		{
			cout << "DB is free" << std::endl;
			return;
		}
		else if (strncmp(command.c_str(), "edit", 4) == 0)
		{
			cout << "Edit\n";
			result = edit(command.c_str());
			if (result == 0)
				return;
		}
		else if (strncmp(command.c_str(), "del", 3) == 0)
		{
			cout << "Del \n";
			result = del(command.c_str());
			if (result == 0)
				return;
		}
		else if (strncmp(command.c_str(), "find", 4) == 0)
		{
			cout << "Find /dev/null\n";
			result = find(command.c_str());
			return;
		}
		else if (strncmp(command.c_str(), "list", 4) == 0)
		{
			cout << "List\n";
			result = list(command.c_str());
			if (result == 0)
				return;
		}

		else
			cout << "Please read list of commands:" << std::endl
				 << "\tadd <...> \n\t- for adding new Rec to DB. \n\t\t(example: add Dyakin Ivan Pavlovich 09.09.1999 Moscow @riokin www.github.com/vvhappyguy)\n"
				 << "\tedit <n> <fieldname> <val> \n\t- for editting <fieldName> field of <n> Rec from db by your value <val> \n\t\t(example: edit 1 Address SergievPosad)\n"
				 << "\tdel <n> \n\t- for deletting <n> Rec from DB \n\t\t(example: del 2)\n";
	}
	return;
}

int main(int args, char *argv[])
{
	DB::Instance().connect();
	DB::Instance().getIF();

	if (args > 1)
	{
		//std::cout << "Arguments[" << args << "]: " << argv[0] << std::endl;
		char ch_command[512];
		sprintf(ch_command, "%s", argv[1]);
		for (ushort i = 2; i < args; i++)
		{
			//std::cout << argv[i] << std::endl;
			sprintf(ch_command, "%s %s", ch_command, argv[i]);
		}
		std::string command = ch_command;
		//std::cout << command << std::endl;
		if (command.find("-it") == std::string::npos)
		{
			std::cout << "CLI-mode. Your command = \"" << command << "\"" << std::endl;
			parse_command(command);
			DB::Instance().closeIF();
			return 0;
		}
		else
		{
			std::cout << "Welcome to Interactive Mode!!!" << std::endl;
			std::string command;
			for (;;)
			{
				cout << "> ";
				std::getline(std::cin, command);
				if (strcmp(command.c_str(), "q") == 0)
					break;
				cout << "Command: " << command << std::endl;
				parse_command(command);
				command = "";
			}
			DB::Instance().closeIF();
			return 0;
		}
	}
}
